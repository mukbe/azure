#include "stdafx.h"
#include "FbxUtility.h"


D3DXVECTOR2 Fbx::Utility::ToVector2(FbxVector2 & vec)
{
	return D3DXVECTOR2((float)vec.mData[0], (float)vec.mData[1]);
}

D3DXVECTOR3 Fbx::Utility::ToVector3(FbxVector4 & vec)
{
	return D3DXVECTOR3((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2]);
}

D3DXCOLOR Fbx::Utility::ToColor(FbxVector4 & vec)
{
	return D3DXCOLOR((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2], 1);
}

D3DXCOLOR Fbx::Utility::ToColor(FbxPropertyT<FbxDouble3>& vec, FbxPropertyT<FbxDouble>& factor)
{
	FbxDouble3 color = vec;

	D3DXCOLOR result;
	result.r = (float)color.mData[0];
	result.g = (float)color.mData[1];
	result.b = (float)color.mData[2];
	result.a = (float)factor;

	return result;
}

D3DXMATRIX Fbx::Utility::ToMatrix(FbxAMatrix & matrix)
{
	FbxVector4 r1 = matrix.GetRow(0);
	FbxVector4 r2 = matrix.GetRow(1);
	FbxVector4 r3 = matrix.GetRow(2);
	FbxVector4 r4 = matrix.GetRow(3);

	return D3DXMATRIX
	(
		(float)r1.mData[0], (float)r1.mData[1], (float)r1.mData[2], (float)r1.mData[3],
		(float)r2.mData[0], (float)r2.mData[1], (float)r2.mData[2], (float)r2.mData[3],
		(float)r3.mData[0], (float)r3.mData[1], (float)r3.mData[2], (float)r3.mData[3],
		(float)r4.mData[0], (float)r4.mData[1], (float)r4.mData[2], (float)r4.mData[3]
	);
}

D3DXVECTOR3 Fbx::Utility::GetNormal(FbxMesh * mesh, int polygonIndex, int vertexIndex)
{
	FbxVector4 fbxNormal(0, 0, 0, 0);
	mesh->GetPolygonVertexNormal(polygonIndex, vertexIndex, fbxNormal);
	fbxNormal.Normalize();

	return ToVector3(fbxNormal);
}

string Fbx::Utility::GetTextureFile(FbxProperty & prop)
{
	if (prop.IsValid() == true)
	{
		if (prop.GetSrcObjectCount() > 0)
		{
			FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>();

			if (texture != nullptr)
			{
				return string(texture->GetFileName());
			}
		}
	}
	return "";
}

string Fbx::Utility::GetMaterialName(FbxMesh * mesh, int polygonIndex, int cpIndex)
{
	//Node의 있는 mat번호를 가져온다 Scene에 있는것과 다른것
	//Scene의 있는 mat의 번호를 순서대로 나열한것


	FbxNode* node = mesh->GetNode();
	if (node == NULL) return "";

	FbxLayerElementMaterial* material = mesh->GetLayer(0)->GetMaterials();
	if (material == NULL) return "";


	FbxLayerElement::EMappingMode mappingMode = material->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = material->GetReferenceMode();


	int mappingIndex = -1;
	switch (mappingMode)
	{
	case FbxLayerElement::eAllSame: mappingIndex = 0; break;
	case FbxLayerElement::eByPolygon: mappingIndex = polygonIndex; break;
	case FbxLayerElement::eByControlPoint: mappingIndex = cpIndex; break;
	case FbxLayerElement::eByPolygonVertex: mappingIndex = polygonIndex * 3; break;
	default: assert(false); break;
	}


	FbxSurfaceMaterial* findMaterial = NULL;
	if (refMode == FbxLayerElement::eDirect)
	{
		if (mappingIndex < node->GetMaterialCount())
			findMaterial = node->GetMaterial(mappingIndex);
	}
	else if (refMode == FbxLayerElement::eIndexToDirect)
	{
		FbxLayerElementArrayTemplate<int>& indexArr = material->GetIndexArray();

		if (mappingIndex < indexArr.GetCount())
		{
			int tempIndex = indexArr.GetAt(mappingIndex);

			if (tempIndex < node->GetMaterialCount())
				findMaterial = node->GetMaterial(tempIndex);
		}//if(mappingIndex)
	}//if(refMode)

	if (findMaterial == NULL)
		return "";

	return findMaterial->GetName();
}
D3DXVECTOR2 Fbx::Utility::GetUv(FbxMesh * mesh, int cpIndex, int uvIndex)
{
	D3DXVECTOR2 result = D3DXVECTOR2(0, 0);
	FbxLayerElementUV* uv = mesh->GetLayer(0)->GetUVs();
	if (uv == nullptr) return result;

	FbxLayerElement::EMappingMode mappingMode = uv->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = uv->GetReferenceMode();

	switch (mappingMode)
	{
	case FbxLayerElement::eNone:
		break;
	case FbxLayerElement::eByControlPoint:
	{
		if (refMode == FbxLayerElement::eDirect)
		{

			result.x = (float)uv->GetDirectArray().GetAt(cpIndex).mData[0];
			result.y = (float)uv->GetDirectArray().GetAt(cpIndex).mData[1];
		}
		else if (refMode == FbxLayerElement::eIndexToDirect)
		{
			//index에 의한
			int index = uv->GetIndexArray().GetAt(cpIndex);

			result.x = (float)uv->GetDirectArray().GetAt(index).mData[0];
			result.y = (float)uv->GetDirectArray().GetAt(index).mData[1];

		}
	}
	break;
	case FbxLayerElement::eByPolygonVertex:
	{
		result.x = (float)uv->GetDirectArray().GetAt(uvIndex).mData[0];
		result.y = (float)uv->GetDirectArray().GetAt(uvIndex).mData[1];
	}
	break;
	case FbxLayerElement::eByPolygon:
		break;
	case FbxLayerElement::eByEdge:
		break;
	case FbxLayerElement::eAllSame:
		break;
	}

	result.y = 1.0 - result.y;

	return result;
}
//ALLSAME일때 가능
//int Fbx::Utility::GetMaterialId(FbxMesh * mesh, int polygonNumber)
//{
//	FbxLayerElementMaterial* materials = mesh->GetLayer(0)->GetMaterials();
//
//	if (materials == nullptr) return -1;
//
//	return materials->GetIndexArray().GetAt(polygonNumber);
//}
