#include "stdafx.h"
#include "InstanceRenderer.h"

#include "Model/Model.h"
#include "Model/ModelMesh.h"
#include "Model/ModelBone.h"
#include "Model/ModelMeshPart.h"
#include "Model/ModelAnimClip.h"


#include "Object/GameObject/GameObject.h"
#include "Renders/Material.h"
#include "./Renders/Instancing/InstanceShader.h"
#include "./Renders/WorldBuffer.h"
#include "./Utilities/Buffer.h"

InstanceRenderer::InstanceRenderer(UINT maxInstance)
	:maxInstanceCount(maxInstance),drawInstanceCout(0)
{
	shader = new InstanceShader(L"./_Shaders/001_GBuffer.hlsl");
}


InstanceRenderer::~InstanceRenderer()
{
	SafeDelete(shader);

	for (ModelMesh* mesh : meshes)
		SafeDelete(mesh);

	for (ModelBone* bone : bones)
		SafeDelete(bone);

	materials.clear();
	meshes.clear();
	bones.clear();
}

void InstanceRenderer::CopyAbsoluteBoneTo()
{
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


void InstanceRenderer::ReadMaterial(wstring file)
{
	Models::LoadMaterial(file, &materials);
}

void InstanceRenderer::ReadMesh(wstring file)
{
	Models::LoadMesh(file, &bones, &meshes);

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
}

void InstanceRenderer::CreateBuffer()
{
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = (sizeof D3DXVECTOR4 * 3) * maxInstanceCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	HRESULT hr = Device->CreateBuffer(&desc, nullptr, &instanceBuffer);
	assert(SUCCEEDED(hr));
}

void InstanceRenderer::UpdateBuffer()
{
	this->drawInstanceCout = 0;

	D3D11_MAPPED_SUBRESOURCE mapData;
	DeviceContext->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapData);
	{
		D3DXMATRIX* dataView = reinterpret_cast<D3DXMATRIX*>(mapData.pData);

		for (UINT i = 0; i < instanceList.size(); ++i)
		{
			dataView[drawInstanceCout] = instanceList[i]->GetFinalMatrix();
			drawInstanceCout++;
		}
	}
	DeviceContext->Unmap(instanceBuffer, 0);
}

void InstanceRenderer::Render()
{
	for (ModelMesh* mesh : meshes)
	{
		int index = mesh->ParentBoneIndex();
		D3DXMATRIX transform = localTransforms[index];

		mesh->SetWorld(transform);
		mesh->GetWorldBuffer()->SetVSBuffer(1);
		for (ModelMeshPart* part : mesh->meshParts)
		{
			UINT stride[2] = { sizeof VertexTextureBlendNT,sizeof D3DXVECTOR4 * 3 };
			UINT offset[2] = { 0,0 };
			ID3D11Buffer* buffers[2] = { part->vertexBuffer,instanceBuffer };

			part->material->BindBuffer();

			DeviceContext->IASetVertexBuffers(0, 2, buffers, stride, offset);
			DeviceContext->IASetIndexBuffer(part->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			shader->Render();

			DeviceContext->DrawIndexedInstanced(part->GetIndexCount(), this->drawInstanceCout, 0, 0, 0);

			part->material->UnBindBuffer();
		}
	}
}

void InstanceRenderer::AddInstanceData(GameObject* object)
{
	if (instanceList.size() >= maxInstanceCount)return;

	this->instanceList.push_back(object);
}
