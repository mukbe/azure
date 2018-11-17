#include "stdafx.h"
#include "InstanceRenderer.h"

#include "Model/Model.h"
#include "Model/ModelMesh.h"
#include "Model/ModelBone.h"
#include "Model/ModelMeshPart.h"
#include "Model/ModelAnimClip.h"


#include "Object/GameObject/GameObject.h"
#include "Object/StaticObject/StaticObject.h"
#include "./Object/GameObject/TagMessage.h"
#include "Renders/Material.h"
#include "./Renders/Instancing/InstanceShader.h"
#include "./Renders/WorldBuffer.h"
#include "./Utilities/Buffer.h"
#include "./Utilities/BinaryFile.h"
#include "./View/FreeCamera.h"
#include "./Bounding/QuadTree/QuadTreeSystem.h"


string InstanceRenderer::Renderer = "InstanceRenderer";

InstanceRenderer::InstanceRenderer(string name,UINT maxInstance)
	:maxInstanceCount(maxInstance), drawInstanceCount(0)
{
	this->name = name;
	shader = new InstanceShader(L"./_Shaders/001_GBuffer.hlsl");
	shadowShader = new InstanceShader(ShaderPath + L"004_Shadow.hlsl");
	this->AddCallback("DeleteObject", [this](TagMessage msg) 
	{
		for (UINT i = 0; i < instanceList.size(); ++i)
		{
			if (instanceList[i]->GetName() == *reinterpret_cast<string*>(msg.data))
			{
				Objects->DeleteObject(ObjectType::Type::Static, ObjectType::Tag::Object, instanceList[i]->GetName());
				instanceList.erase(instanceList.begin() + i);
				break;
			}
		}
	});
	this->AddCallback("SaveMaterial", [this](TagMessage msg) {this->SaveMaterial(); });

	string newName = name;
	String::Replace(&newName, InstanceRenderer::Renderer, "");
	this->InitializeData(newName);
}

InstanceRenderer::InstanceRenderer(string name, Json::Value* parent)
	: drawInstanceCount(0),maxInstanceCount(0)
{
	this->name = name;
	shader = new InstanceShader(L"./_Shaders/001_GBuffer.hlsl");
	shadowShader = new InstanceShader(ShaderPath + L"004_Shadow.hlsl");
	this->AddCallback("DeleteObject", [this](TagMessage msg)
	{
		for (UINT i = 0; i < instanceList.size(); ++i)
		{
			if (instanceList[i]->GetName() == *reinterpret_cast<string*>(msg.data))
			{
				Objects->DeleteObject(ObjectType::Type::Static, ObjectType::Tag::Object, instanceList[i]->GetName());
				instanceList.erase(instanceList.begin() + i);
				break;
			}
		}
	});
	this->AddCallback("SaveMaterial", [this](TagMessage msg) {this->SaveMaterial(); });

	string newName = name;
	String::Replace(&newName, InstanceRenderer::Renderer, "");

	ModelData data = AssetManager->GetModelData(newName);
	vector<ModelAnimClip*> temp;
	data.Clone(&materials, &bones, &meshes, &temp, &colliders);
	temp.clear();
	this->BindMeshData();
	this->LoadData(parent);
	this->CreateBuffer();
	this->UpdateBuffer();

	
	QuadTreeSystem* quadTree = (QuadTreeSystem*)Objects->FindObject(ObjectType::Type::Dynamic, ObjectType::Tag::System, "QuadTreeSystem");
	if (quadTree) {
		for (UINT i = 0; i < instanceList.size(); ++i)
			((StaticObject*)instanceList[i])->AttachQuadTree(quadTree);
	}
}


InstanceRenderer::~InstanceRenderer()
{
	
}

void InstanceRenderer::BindMeshData()
{
	for (ModelMesh* mesh : meshes)
	{
		for (ModelMeshPart* part : mesh->meshParts)
			part->material = GetMaterialByName(part->materialName);
	}


	this->root = bones[0];
	for (ModelBone* bone : bones)
	{
		if (bone->parentIndex > -1)
		{
			bone->parent = bones[bone->parentIndex];
			bone->parent->childs.push_back(bone);
		}
		else
			bone->parent = nullptr;
	}

	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i]->parentBone = bones[meshes[i]->parentBoneIndex];
		meshes[i]->Binding();
	}

	localTransforms.clear();
	localTransforms.assign(bones.size(), D3DXMATRIX());

	for (size_t i = 0; i < bones.size(); i++)
	{
		ModelBone* bone = bones[i];

		if (bone->parent != NULL)
		{
			int index = bone->parent->index;
			localTransforms[i] = bone->transform * localTransforms[index];
		}
		else
		{
			localTransforms[i] = bone->transform;
		}
	}
}

Material * InstanceRenderer::GetMaterialByName(wstring name)
{
	for (Material* material : materials)
	{
		if (material->GetName() == name)
			return material;
	}

	return nullptr;
}




void InstanceRenderer::CreateBuffer()
{
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = (sizeof InstanceData) * maxInstanceCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	HRESULT hr = Device->CreateBuffer(&desc, nullptr, &instanceBuffer);
	assert(SUCCEEDED(hr));
}

void InstanceRenderer::InitializeData(string keyName)
{
	ModelData data = AssetManager->GetModelData(keyName);
	vector<ModelAnimClip*> temp;
	data.Clone(&materials, &bones, &meshes,&temp,&colliders);
	temp.clear();

	this->BindMeshData();
	this->CreateBuffer();
}

void InstanceRenderer::Release()
{
	SafeDelete(shader);
	SafeDelete(shadowShader);

	for (ModelMesh* mesh : meshes)
		SafeDelete(mesh);

	for (ModelBone* bone : bones)
		SafeDelete(bone);

	for (GameCollider* collider : colliders)
		SafeDelete(collider);

	materials.clear();
	meshes.clear();
	bones.clear();
	colliders.clear();

	localTransforms.clear();
	instanceList.clear();

	SafeRelease(instanceBuffer);
}

void InstanceRenderer::PreUpdate()
{
	//for (UINT i = 0; i < instanceList.size(); ++i)
	//	instanceList[i]->SetIsRender(false);
}

void InstanceRenderer::PostUpdate()
{
	this->UpdateBuffer();
}



void InstanceRenderer::UpdateBuffer()
{
	this->drawInstanceCount = 0;

	D3D11_MAPPED_SUBRESOURCE mapData;
	DeviceContext->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapData);
	{
		InstanceData* dataView = reinterpret_cast<InstanceData*>(mapData.pData);

		for (UINT i = 0; i < instanceList.size(); ++i)
		{
			if (instanceList[i]->GetIsRender())
			{
				D3DXMATRIX mat = instanceList[i]->GetFinalMatrix();
				dataView[drawInstanceCount].data[0] = D3DXVECTOR4(mat._11, mat._12, mat._13, mat._41);
				dataView[drawInstanceCount].data[1] = D3DXVECTOR4(mat._21, mat._22, mat._23, mat._42);
				dataView[drawInstanceCount].data[2] = D3DXVECTOR4(mat._31, mat._32, mat._33, mat._43);
				drawInstanceCount++;
			}
		}
	}
	DeviceContext->Unmap(instanceBuffer, 0);
}

void InstanceRenderer::ShadowRender()
{
	States::SetSampler(0, States::SamplerStates::LINEAR_MIRROR);
	for (ModelMesh* mesh : meshes)
	{
		int index = mesh->ParentBoneIndex();
		D3DXMATRIX transform = localTransforms[index];

		mesh->SetWorld(transform);
		mesh->GetWorldBuffer()->SetVSBuffer(1);

		for (ModelMeshPart* part : mesh->meshParts)
		{
			UINT stride[2] = { sizeof VertexTextureBlendNT,sizeof InstanceData };
			UINT offset[2] = { 0,0 };
			ID3D11Buffer* buffers[2] = { part->vertexBuffer,instanceBuffer };

			part->material->UpdateBuffer();
			part->material->BindBuffer();

			DeviceContext->IASetVertexBuffers(0, 2, buffers, stride, offset);
			DeviceContext->IASetIndexBuffer(part->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			shadowShader->Render();

			DeviceContext->DrawIndexedInstanced(part->GetIndexCount(), this->drawInstanceCount, 0, 0, 0);

			part->material->UnBindBuffer();
		}
	}
	States::SetSampler(0, States::SamplerStates::LINEAR);
}


void InstanceRenderer::Render()
{
	States::SetSampler(0, States::SamplerStates::LINEAR_MIRROR);
	for (ModelMesh* mesh : meshes)
	{
		int index = mesh->ParentBoneIndex();
		D3DXMATRIX transform = localTransforms[index];

		mesh->SetWorld(transform);
		mesh->GetWorldBuffer()->SetVSBuffer(1);
		for (ModelMeshPart* part : mesh->meshParts)
		{
			UINT stride[2] = { sizeof VertexTextureBlendNT,sizeof InstanceData};
			UINT offset[2] = { 0,0 };
			ID3D11Buffer* buffers[2] = { part->vertexBuffer,instanceBuffer };
			
			part->material->UpdateBuffer();
			part->material->BindBuffer();

			DeviceContext->IASetVertexBuffers(0, 2, buffers, stride, offset);
			DeviceContext->IASetIndexBuffer(part->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			shader->Render();

			DeviceContext->DrawIndexedInstanced(part->GetIndexCount(), this->drawInstanceCount, 0, 0, 0);

			part->material->UnBindBuffer();
		}
	}
	States::SetSampler(0, States::SamplerStates::LINEAR);

}

void InstanceRenderer::UIRender()
{
	static float autoSize = 0.05f;
	ImGui::Text("InstanceRendering");
	ImGui::Text("DrawCount : %d", drawInstanceCount);
	ImGui::InputFloat("CreateSize", &autoSize);

	if (ImGui::Button("AddInstance", ImVec2(100, 20)))
		this->AddInstance(autoSize);
	ImGui::SameLine();
	if (ImGui::Button("SaveMaterial", ImVec2(100, 20)))
		this->SendMSG(TagMessage("SaveMaterial",0.1f));

	ImGui::Separator();
	
	for (UINT i = 0; i < materials.size(); ++i)
	{
		string name = String::WStringToString(materials[i]->GetName());
		if (ImGui::TreeNode(name.c_str()))
		{
			materials[i]->UIRender();
			ImGui::TreePop();
		}
	}
}

void InstanceRenderer::UIUpdate()
{

}

void InstanceRenderer::AddInstanceData(GameObject* object)
{
	if (instanceList.size() >= maxInstanceCount)return;

	this->instanceList.push_back(object);
}

void InstanceRenderer::SaveData()
{
	wstring file = L"../_Scenes/Scene01/" + String::StringToWString(name) + L".data";

	UINT size = instanceList.size(); 

	BinaryWriter* w = new BinaryWriter();
	w->Open(file);
	{
		w->UInt(size);
		for (UINT i = 0; i < instanceList.size(); ++i)
		{
			string instanceName = instanceList[i]->GetName();
			w->String(instanceName);
			w->Byte(&instanceList[i]->GetFinalMatrix(), sizeof D3DXMATRIX);
		}
	}
	w->Close();

	SafeDelete(w);
}

void InstanceRenderer::LoadData(wstring file)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);
	{
		this->maxInstanceCount = r->UInt();

		for (UINT i = 0; i < maxInstanceCount; ++i)
		{
			StaticObject* object = new StaticObject(r->String());
			void* byte; D3DXMATRIX mat;
			byte = &mat;
			r->Byte(&byte, sizeof D3DXMATRIX);
			object->GetTransform()->SetTransform(mat);
			for (UINT i = 0; i < colliders.size(); ++i)
				object->AddCollider(colliders[i]);
			object->SetInstanceRenderer(this);
			this->AddInstanceData(object);
			Objects->AddObject(ObjectType::Type::Static, ObjectType::Tag::Object, object);
		}
	}
	r->Close();
	SafeDelete(r);

	if (maxInstanceCount < 20)
		maxInstanceCount = 20;
}

void InstanceRenderer::SaveData(Json::Value * parent)
{
	//Binary
	this->SaveData();
	string newName = name;
	String::Replace(&newName, InstanceRenderer::Renderer, "");
	wstring filePath = AssetManager->GetModelData(newName).file;
	this->SaveMaterial(filePath + L".material");

	wstring file = L"../_Scenes/Scene01/" + String::StringToWString(name) + L".data";

	Json::Value value;
	{
		JsonHelper::SetValue(value, "Name", this->name);
		JsonHelper::SetValue(value, "FileName", String::WStringToString(file));
		JsonHelper::SetValue(value, "IsActive", isActive);
	}
	(*parent)[this->name.c_str()] = value;
}

void InstanceRenderer::LoadData(Json::Value * parent)
{
	string file;
	GameObject::LoadData(parent);
	JsonHelper::GetValue(*parent, "FileName", file);
	this->LoadData(String::StringToWString(file));
}

void InstanceRenderer::AddInstance(float autoSize)
{
	char str[48];
	string newName = name;
	String::Replace(&newName, "Renderer", "");
	sprintf_s(str, "%s %d", newName.c_str(), instanceList.size());

	StaticObject* object = new StaticObject(str);
	object->GetTransform()->SetWorldPosition(MainCamera->GetTransform()->GetWorldPosition() +
		MainCamera->GetTransform()->GetForward() * 50.0f);
	object->GetTransform()->SetScale(autoSize, autoSize, autoSize);

	for (UINT i = 0; i < colliders.size(); ++i)
		object->AddCollider(colliders[i]);
	object->SetInstanceRenderer(this);
	this->AddInstanceData(object);
	Objects->AddObject(ObjectType::Type::Static, ObjectType::Tag::Object, object);
}

void InstanceRenderer::SaveMaterial(wstring file)
{
	if (file.length() > 0)
	{
		Json::Value root;
		for (UINT i = 0; i < materials.size(); ++i)
		{
			Material* material = materials[i];

			Json::Value val;
			JsonHelper::SetValue(val, "Name", String::WStringToString(material->GetName()));

			JsonHelper::SetValue(val, "Ambient", material->GetAmbientColor());
			JsonHelper::SetValue(val, "Diffuse", material->GetDiffuseColor());
			JsonHelper::SetValue(val, "Emissive", material->GetEmissiveColor());
			JsonHelper::SetValue(val, "Specular", material->GetSpecColor());
			float f = material->GetShiness();
			JsonHelper::SetValue(val, "Shininess", f);

			string nullString = "";
			string file = "";

			if (material->GetDiffuseMap())
				file = String::WStringToString(material->GetDiffuseMap()->GetFilePath());
			else
				file = nullString;

			JsonHelper::SetValue(val, "DiffuseFile", file);

			if (material->GetSpecularMap())
				file = String::WStringToString(material->GetSpecularMap()->GetFilePath());
			else
				file = nullString;

			JsonHelper::SetValue(val, "SpecularFile", nullString);

			JsonHelper::SetValue(val, "EmissiveFile", nullString);

			if (material->GetNormalMap())
				file = String::WStringToString(material->GetNormalMap()->GetFilePath());
			else
				file = nullString;

			JsonHelper::SetValue(val, "NormalFile", file);

			JsonHelper::SetValue(val, "DetailFile", nullString);

			root[String::WStringToString(material->GetName()).c_str()] = val;
		}

		JsonHelper::WriteData(file, &root);
	}
	else
	{
		function<void(wstring)> func = bind(&InstanceRenderer::SaveMaterial, this, placeholders::_1);
		Path::SaveFileDialog(L"", Path::MaterialFilter, Assets, func);
	}
}
