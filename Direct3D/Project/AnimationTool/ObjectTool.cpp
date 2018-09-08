#include "stdafx.h"
#include "ObjectTool.h"

#include "./Bounding/BoundingBox.h"
#include "./Bounding/GameCollider.h"

#include "./Model/Model.h"
#include "./Model/ModelMesh.h"
#include "./Model/ModelMeshPart.h"
#include "./Model/ModelBone.h"

#include "./Project/AnimationTool/FBX/Exporter.h"

#include "./Renders/Material.h"

#include "./Utilities/Transform.h"
#include "./Utilities/DebugTransform.h"
#include "./Utilities/Path.h"
#include "./Utilities/BinaryFile.h"
#include "./Utilities/ImGuiHelper.h"

#include "./View/FreeCamera.h"


ObjectTool::ObjectTool()
	:model(nullptr), targetCollider(nullptr), zBufferOff(false)
{
	//Fbx::Exporter* exporter = new Fbx::Exporter(L"../_Assets/Objects/Trees/Tree6.fbx");
	//exporter->ExportMaterial(L"../_Assets/Objects/Trees/", L"Tree6");
	//exporter->ExportMesh(L"../_Assets/Objects/Trees/", L"Tree6");
	//SafeDelete(exporter);

	debugTransform = new DebugTransform;
	debugTransform->ConnectTransform(new Transform);

	shader = new Shader(ShaderPath + L"001_GBuffer.hlsl", Shader::ShaderType::Default, "Object");
}


ObjectTool::~ObjectTool()
{
	this->ReleaseCollider();

	SafeDelete(shader);
	SafeDelete(debugTransform);
	SafeDelete(model);
}

void ObjectTool::Update()
{
	if (model)
	{
		if (targetCollider)
		{
			debugTransform->Update();
			targetCollider->SetFinalMatrix(debugTransform->GetTransform()->GetFinalMatrix());
		}
	}
}

void ObjectTool::Render()
{
	if (model)
	{
		vector<D3DXMATRIX> localTransform;
		model->CopyAbsoluteBoneTo(localTransform);

		for (ModelMesh* mesh : model->Meshes())
		{
			int index = mesh->ParentBoneIndex();
			D3DXMATRIX transform = localTransform[index];

			mesh->SetWorld(transform);
			mesh->GetWorldBuffer()->SetVSBuffer(1);

			for (ModelMeshPart* part : mesh->GetMeshParts())
			{
				UINT stride = sizeof VertexTextureBlendNT;
				UINT offset = 0;

				ID3D11Buffer* buffer = part->GetVertexBuffer();

				part->GetMaterial()->BindBuffer();

				DeviceContext->IASetVertexBuffers(0, 1, &buffer,& stride, &offset);
				DeviceContext->IASetIndexBuffer(part->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
				DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				shader->Render();

				DeviceContext->DrawIndexed(part->GetIndexCount(), 0, 0);

				part->GetMaterial()->UnBindBuffer();
			}
		}

		if (targetCollider)
		{
			debugTransform->RenderGizmo();
		}


		for (UINT i = 0; i < colliderList.size(); ++i)
		{
			if (colliderList[i] == targetCollider)
				targetCollider->Render(ColorWhite, zBufferOff);
			else
				colliderList[i]->Render(ColorRed, zBufferOff);
		}
	}
}

void ObjectTool::UIRender()
{
	AssetManager->UIRender();

	if (ImGui::Begin("ObjectTool"))
	{
		function<void(ModelData*)> func = bind(&ObjectTool::GetModelData, this, placeholders::_1);
		string name;
		if (model)name = "Model";
		else name = "Null";
		ImGuiHelper::RenderModelSelectable(name.c_str(), func);

		if (ImGui::Button("AddCollider", ImVec2(100, 20)))
			ImGui::OpenPopup("AddCollider");
		ImGui::SameLine();
		if (ImGui::Button("DeleteCollider", ImVec2(100, 20)))
			this->DeleteCollider();
		ImGui::SameLine();
		if (ImGui::Button("CreateAutoCollider", ImVec2(100, 20)))
			ImGui::OpenPopup("AutoCollider");


		if (ImGui::Button("SaveCollider", ImVec2(100, 20)))
			this->SaveCollider();
		ImGui::SameLine();
		if (ImGui::Button("LoadCollider", ImVec2(100, 20)))
			this->LoadCollider();

		ImGui::Checkbox("ZOn", &zBufferOff);

		ImGui::Separator();

		//BeginCombo ColliderList ---------------------------------------------------------------
		static string comboStr = "NULL";
		if (targetCollider)comboStr = targetCollider->GetName();
		else comboStr = "NULL";
		if (ImGui::BeginCombo("ColliderList", comboStr.c_str()))
		{
			for (UINT i = 0; i < colliderList.size(); ++i)
			{
				bool isSelected = false;
				if (ImGui::Selectable(colliderList[i]->GetName().c_str(), isSelected))
				{
					comboStr = colliderList[i]->GetName();
					this->ChangeTargetCollider(colliderList[i]);
				}
				if (isSelected);
				ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		//-----------------------------------------------------------------------------------
		this->AddCollider();
		this->ChoosePart();

		ImGui::End();
	}

	if (targetCollider)
		debugTransform->RenderGUI();
}

void ObjectTool::SetCamera(FreeCamera * camera)
{
	debugTransform->SetCamera(camera);
}



void ObjectTool::LoadMaterial(wstring file)
{
	if (file.length() > 0)
	{
		model->ReadMaterial(file);
	}
	else
	{
		function<void(wstring)> func = bind(&ObjectTool::LoadMaterial, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::MaterialFilter, Assets, func);
	}
}

void ObjectTool::LoadMesh(wstring file)
{
	if (file.length() > 0)
	{
		model->ReadMesh(file);
	}
	else
	{
		function<void(wstring)> func = bind(&ObjectTool::LoadMesh, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::MeshFilter, Assets, func);
	}
}

void ObjectTool::ChoosePart()
{
	if (ImGui::BeginPopup("AutoCollider"))
	{
		static string comboStr = "NULL";
		static ModelMeshPart* targetMesh = nullptr;
		if (ImGui::BeginCombo("PartList", comboStr.c_str()))
		{
			for (ModelMesh* mesh : model->Meshes())
			{
				for (ModelMeshPart* part : mesh->GetMeshParts())
				{
					bool isSelected = false;
					if (ImGui::Selectable(String::WStringToString(part->GetMaterialName()).c_str(), isSelected))
					{
						comboStr = String::WStringToString(part->GetMaterialName());
						targetMesh = part;
					}
					if (isSelected);
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();
		
		if (ImGui::Button("Accept",ImVec2(80,30)))
		{
			this->CreateColliderAuto(targetMesh);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancle", ImVec2(80, 30)))
			ImGui::CloseCurrentPopup();


		ImGui::EndPopup();
	}
}

void ObjectTool::CreateColliderAuto(ModelMeshPart* part)
{
	if (model)
	{
		vector<VertexTextureBlendNT> verticies = part->GetVertexData();
		D3DXVECTOR3 min, max;
		D3DXComputeBoundingBox(&verticies.begin()->position, verticies.size(), sizeof VertexTextureBlendNT, &min, &max);

		GameCollider* collider = new GameCollider(nullptr, new BoundingBox(min, max));
		colliderList.push_back(collider);
		this->ChangeTargetCollider(collider);
	}
}

void ObjectTool::AddCollider()
{
	if (ImGui::BeginPopup("AddCollider"))
	{
		static bool isSelected = false;
		static int selectType; 0;
		static char str[48] = {};
		ImGui::InputText("Name", str, sizeof(char)* 48);

		if (ImGui::BeginCombo("BoundingType", GameCollider::GetTypeName(selectType).c_str()))
		{
			if (ImGui::Selectable("RigidBody", &isSelected))selectType = 0;
			if (ImGui::Selectable("Attack", &isSelected))selectType = 1;
			if (ImGui::Selectable("HeatBox", &isSelected))selectType = 2;
			if (ImGui::Selectable("Static", &isSelected))selectType = 3;

			if (isSelected);
			ImGui::SetItemDefaultFocus();

			ImGui::EndCombo();
		}

		ImGui::Separator();

		if (ImGui::Button("Add", ImVec2(70, 20)))
		{
			GameCollider* collider = new GameCollider(nullptr,new BoundingBox);
			collider->SetName(str);
			collider->SetType((GameCollider::ColliderType)selectType);

			this->colliderList.push_back(collider);
			this->ChangeTargetCollider(collider);

			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancle", ImVec2(70, 20)))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}

void ObjectTool::DeleteCollider()
{
	if (targetCollider)
	{
		for (UINT i = 0; i < colliderList.size(); ++i)
		{
			if (colliderList[i] == targetCollider)
			{
				SafeDelete(colliderList[i]);
				colliderList.erase(colliderList.begin() + i);

				if (colliderList.size() > 0)
					ChangeTargetCollider(colliderList[0]);
				else
					ChangeTargetCollider(nullptr);
				break;
			}
		}
	}
}

void ObjectTool::SaveCollider(wstring file)
{
	if (file.length() > 0)
	{
		BinaryWriter* w = new BinaryWriter;
		w->Open(file);
		{
			w->UInt(colliderList.size());
			for (UINT i = 0; i < colliderList.size(); ++i)
				GameCollider::SaveCollider(w, colliderList[i]);
		}
		w->Close();
		SafeDelete(w);
	}
	else
	{
		function<void(wstring)> func = bind(&ObjectTool::SaveCollider, this, placeholders::_1);
		Path::SaveFileDialog(file, Path::ColliderFilter, Assets, func);
	}
}

void ObjectTool::LoadCollider(wstring file)
{
	if (file.length() > 0)
	{
		ReleaseCollider();
		BinaryReader* r = new BinaryReader;
		r->Open(file);
		{
			UINT size = r->UInt();
			for (UINT i = 0; i < size; ++i)
			{
				GameCollider* collider = new GameCollider(nullptr,new BoundingBox);
				GameCollider::LoadCollider(r, collider);
				this->colliderList.push_back(collider);
			}
		}
		r->Close();
		SafeDelete(r);
		this->ChangeTargetCollider(this->colliderList[0]);
	}
	else
	{
		function<void(wstring)> func = bind(&ObjectTool::LoadCollider, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::ColliderFilter, Assets, func);
	}
}

void ObjectTool::ReleaseCollider()
{
	for (UINT i = 0; i < colliderList.size(); ++i)
		SafeDelete(colliderList[i]);
	colliderList.clear();

	targetCollider = nullptr;
}

void ObjectTool::ChangeTargetCollider(GameCollider * collider)
{
	this->targetCollider = collider;
	if(collider)
		this->debugTransform->GetTransform()->SetTransform(collider->GetFinalMatrix());
	
}

void ObjectTool::GetModelData(ModelData * pData)
{
	ReleaseCollider();
	SafeDelete(model);
	model = new Model;
	pData->Clone(model);
	model->BindMeshData();
}