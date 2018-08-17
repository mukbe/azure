#include "stdafx.h"
#include "ModelMesh.h"

#include "ModelMeshPart.h"
#include "ModelBone.h"

#include "../Renders/WorldBuffer.h"

ModelMesh::ModelMesh()
{
	worldBuffer = Buffers->FindShaderBuffer<WorldBuffer>();
}


ModelMesh::~ModelMesh()
{
	for (ModelMeshPart* part : meshParts)
		SafeDelete(part);
}


void ModelMesh::Clone(void ** clone)
{
	ModelMesh* mesh = new ModelMesh();
	mesh->name = name;
	mesh->parentBoneIndex = parentBoneIndex;

	for (size_t i = 0; i < meshParts.size(); i++)
	{
		ModelMeshPart*  meshPart = nullptr;
		meshParts[i]->Clone((void**)&meshPart);

		mesh->meshParts.push_back(meshPart);
	}
	*clone = mesh;
}

void ModelMesh::SetWorld(D3DXMATRIX & world)
{
	this->worldBuffer->SetMatrix(world);
}

void ModelMesh::Render(Shader * shader)
{
	for (ModelMeshPart* part : meshParts)
	{
		worldBuffer->SetVSBuffer(1);

		part->Render(shader);
	}
}

void ModelMesh::Binding()
{
	for (ModelMeshPart* part : meshParts)
		part->Binding();
}

