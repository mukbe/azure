#pragma once
#include "stdafx.h"

namespace Fbx
{
	struct FbxMaterial
	{
		string Name;

		D3DXCOLOR Ambient;
		D3DXCOLOR Emissive;
		D3DXCOLOR Diffuse;
		D3DXCOLOR Specular;

		float Shininess;

		string DiffuseFile;
		string SpecularFile;
		string EmissiveFile;
		string NormalFile;
	};

	struct FbxVertex
	{
		int ControlPoint;
		string MaterialName;

		VertexTextureBlendNT Vertex;
	};


	struct FbxBoneData
	{
		int Index;
		string Name;

		int Parent;

		D3DXMATRIX Transform;
		D3DXMATRIX AbsoluteTransform;
	};

	struct FbxMeshPartData
	{
		string MaterialName;

		vector<VertexTextureBlendNT> Vertices;
		vector<UINT> Indices;
	};

	struct FbxMeshData
	{
		string Name;
		int ParentBone;

		FbxMesh* Mesh;
		vector<FbxVertex*> Vertex;

		D3DXMATRIX Transform;
		D3DXMATRIX AbsoluteTransform;

		vector<FbxMeshPartData *> MeshParts;
	};

	struct FbxKeyframeData
	{
		D3DXMATRIX Transform;
		D3DXQUATERNION Rotation;
		D3DXVECTOR3 Translation;
		D3DXVECTOR3 Scale;
	};

	struct FbxKeyframe
	{
		string BoneName;
		vector<FbxKeyframeData> Transforms;
	};

	struct FbxAnimation
	{
		string Name;

		int TotalFrame;
		float FrameRate;

		vector<FbxKeyframe *> Keyframes;
	};
}