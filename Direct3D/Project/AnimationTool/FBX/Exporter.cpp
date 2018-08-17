#include "stdafx.h"
#include "Exporter.h"

#include "Type.h"
#include "BoneWeights.h"
#include "Fbxutility.h"
#include "./Utilities/BinaryFile.h"
#include "./Utilities/String.h"
#include "./Utilities/Path.h"
#include "./Utilities/Json.h"


Fbx::Exporter::Exporter(wstring file)
{
	manager = FbxManager::Create();
	scene = FbxScene::Create(manager, "");

	ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);


	importer = FbxImporter::Create(manager, "");

	string sFile = String::WStringToString(file);
	bool bCheck = importer->Initialize(sFile.c_str(), -1, ios);
	assert(bCheck == true);

	bCheck = importer->Import(scene);
	assert(bCheck == true);


	converter = new FbxGeometryConverter(manager);
}

Fbx::Exporter::~Exporter()
{
	SafeDelete(converter);

	ios->Destroy();
	importer->Destroy();
	scene->Destroy();
	manager->Destroy();
}

void Fbx::Exporter::ExportMaterial(wstring saveFolder, wstring saveName)
{
	ReadMaterial();

	WriteMaterial(saveFolder, saveName);
}

void Fbx::Exporter::ExportMesh(wstring saveFolder, wstring saveName)
{
	ReadBoneData(scene->GetRootNode(), -1, -1);

	ReadSkinData();

	WriteMeshData(saveFolder, saveName);
}

void Fbx::Exporter::ExportAnimation(wstring saveFolder, wstring saveName)
{
	ReadAnimation();

	WriteAnimation(saveFolder, saveName);
}

void Fbx::Exporter::ReadMaterial()
{
	int count = scene->GetMaterialCount();

	for (int i = 0; i < count; i++)
	{
		FbxSurfaceMaterial* fbxMaterial = scene->GetMaterial(i);

		FbxMaterial* material = new FbxMaterial();
		material->Name = fbxMaterial->GetName();

		if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) == true)
		{
			FbxSurfaceLambert* lambert = (FbxSurfaceLambert *)fbxMaterial;

			material->Ambient = Utility::ToColor(lambert->Ambient, lambert->AmbientFactor);
			material->Emissive = Utility::ToColor(lambert->Emissive, lambert->EmissiveFactor);

			material->Diffuse = Utility::ToColor(lambert->Diffuse, lambert->DiffuseFactor);
			material->Diffuse.a = (float)lambert->TransparencyFactor;
		}

		if (fbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId) == true)
		{
			FbxSurfacePhong* phong = (FbxSurfacePhong *)fbxMaterial;

			material->Specular = Utility::ToColor(phong->Specular, phong->SpecularFactor);
			material->Shininess = (float)phong->Shininess;
		}


		FbxProperty prop;

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		material->DiffuseFile = Utility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
		material->SpecularFile = Utility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
		material->EmissiveFile = Utility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sBump);
		material->NormalFile = Utility::GetTextureFile(prop);

		materials.push_back(material);
	}
}

void Fbx::Exporter::WriteMaterial(wstring saveFolder, wstring saveName)
{
	if (Path::ExistDirectory(saveFolder) == false)
		CreateDirectory(saveFolder.c_str(), NULL);


	Json::Value root;
	for (FbxMaterial* material : materials)
	{
		Json::Value val;
		JsonHelper::SetValue(val, "Name", material->Name);

		JsonHelper::SetValue(val, "Ambient", material->Ambient);
		JsonHelper::SetValue(val, "Diffuse", material->Diffuse);
		JsonHelper::SetValue(val, "Emissive", material->Emissive);
		JsonHelper::SetValue(val, "Specular", material->Specular);
		JsonHelper::SetValue(val, "Shininess", material->Shininess);

		CopyTextureFile(material->DiffuseFile, saveFolder);
		JsonHelper::SetValue(val, "DiffuseFile", material->DiffuseFile);

		CopyTextureFile(material->SpecularFile, saveFolder);
		JsonHelper::SetValue(val, "SpecularFile", material->SpecularFile);

		CopyTextureFile(material->EmissiveFile, saveFolder);
		JsonHelper::SetValue(val, "EmissiveFile", material->EmissiveFile);

		CopyTextureFile(material->NormalFile, saveFolder);
		JsonHelper::SetValue(val, "NormalFile", material->NormalFile);

		root[material->Name.c_str()] = val;

		SafeDelete(material);
	}

	JsonHelper::WriteData(saveFolder + saveName + L".material", &root);
}

void Fbx::Exporter::CopyTextureFile(string & textureFile, wstring saveFolder)
{
	if (textureFile.length() < 1)
		return;

	wstring file = String::StringToWString(textureFile);
	wstring fileName = Path::GetFileName(file);

	if (Path::ExistFile(textureFile) == true)
		CopyFile(file.c_str(), (saveFolder + fileName).c_str(), FALSE);

	textureFile = String::WStringToString(fileName);
}

///////////////////////////////////////////////////////////////////////////////

void Fbx::Exporter::ReadBoneData(FbxNode * node, int index, int parent)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();

		bool bCheck = false;
		bCheck |= (nodeType == FbxNodeAttribute::eSkeleton);
		bCheck |= (nodeType == FbxNodeAttribute::eMarker);
		bCheck |= (nodeType == FbxNodeAttribute::eNull);
		bCheck |= (nodeType == FbxNodeAttribute::eMesh);

		if (bCheck == true)
		{
			FbxBoneData* bone = new FbxBoneData();
			bone->Index = index;
			bone->Parent = parent;
			bone->Name = node->GetName();
			bone->Transform = Utility::ToMatrix(node->EvaluateLocalTransform());
			bone->AbsoluteTransform = Utility::ToMatrix(node->EvaluateGlobalTransform());

			boneDatas.push_back(bone);

			if (nodeType == FbxNodeAttribute::eMesh)
			{
				converter->Triangulate(attribute, true, true);

				ReadMeshData(node, index);

			}
		}
	}

	for (int i = 0; i < node->GetChildCount(); i++)
		ReadBoneData(node->GetChild(i), boneDatas.size(), index);
}

void Fbx::Exporter::ReadMeshData(FbxNode * node, int parentBone)
{

	FbxMesh* mesh = node->GetMesh();



	vector<FbxVertex *> vertices;
	for (int p = 0; p < mesh->GetPolygonCount(); p++)
	{
		int vertexInPolygonCount = mesh->GetPolygonSize(p);
		assert(vertexInPolygonCount == 3);

		for (int vi = 0; vi < vertexInPolygonCount; vi++)
		{
			FbxVertex* vertex = new FbxVertex();

			int cpIndex = mesh->GetPolygonVertex(p, vi);
			vertex->ControlPoint = cpIndex;

			FbxVector4 position = mesh->GetControlPoints()[cpIndex];
			vertex->Vertex.position = Utility::ToVector3(position);

			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(p, vi, normal);
			normal.Normalize();
			vertex->Vertex.normal = Utility::ToVector3(normal);

			vertex->MaterialName = Utility::GetMaterialName(mesh, p, cpIndex);

			int uvIndex = mesh->GetTextureUVIndex(p, vi);
			vertex->Vertex.uv = Utility::GetUv(mesh, cpIndex, uvIndex);

			vertices.push_back(vertex);
		}//for(vi)
	}//for(p)


	FbxMeshData* meshData = new FbxMeshData();
	meshData->Name = node->GetName();
	meshData->ParentBone = parentBone;
	meshData->Mesh = mesh;
	meshData->Vertex.assign(vertices.begin(), vertices.end());

	FbxAMatrix transform = node->EvaluateLocalTransform();
	meshData->Transform = Utility::ToMatrix(transform);

	FbxAMatrix absoluteTransform = node->EvaluateGlobalTransform();
	meshData->AbsoluteTransform = Utility::ToMatrix(absoluteTransform);

	meshDatas.push_back(meshData);

}

void Fbx::Exporter::ReadSkinData()
{
	for (FbxMeshData* meshData : meshDatas)
	{
		FbxMesh* mesh = meshData->Mesh;



		///////////////////////////////////////////////////////////////////////////////////////

		int deformerCount = mesh->GetDeformerCount();
		vector<FbxBoneWeights> boneWeights(mesh->GetControlPointsCount(), FbxBoneWeights());

		for (int i = 0; i < deformerCount; i++)
		{
			FbxDeformer* deformer = mesh->GetDeformer(i, FbxDeformer::eSkin);

			FbxSkin* skin = reinterpret_cast<FbxSkin *>(deformer);
			if (skin == NULL) continue;

			for (int clusterIndex = 0; clusterIndex < skin->GetClusterCount(); clusterIndex++)
			{
				FbxCluster* cluster = skin->GetCluster(clusterIndex);
				assert(cluster->GetLinkMode() == FbxCluster::eNormalize);

				string linkName = cluster->GetLink()->GetName();
				UINT boneIndex = GetBoneIndexByName(linkName);


				FbxAMatrix transform;
				FbxAMatrix linkTransform;

				cluster->GetTransformMatrix(transform);
				cluster->GetTransformLinkMatrix(linkTransform);

				boneDatas[boneIndex]->Transform = Utility::ToMatrix(transform);
				boneDatas[boneIndex]->AbsoluteTransform = Utility::ToMatrix(linkTransform);


				for (int indexCount = 0; indexCount < cluster->GetControlPointIndicesCount(); indexCount++)
				{
					int temp = cluster->GetControlPointIndices()[indexCount];
					double* weights = cluster->GetControlPointWeights();

					boneWeights[temp].AddBoneWeight(boneIndex, (float)weights[indexCount]);
				}
			}//for(clusterIndex)
		}//for(joints)

		for (size_t i = 0; i < boneWeights.size(); i++)
			boneWeights[i].Normalize();

		for (FbxVertex* vertex : meshData->Vertex)
		{
			int cpIndex = vertex->ControlPoint;

			FbxBlendWeight weights;
			boneWeights[cpIndex].GetBlendWeights(weights);
			vertex->Vertex.blendIndices = weights.Indices;
			vertex->Vertex.blendWeights = weights.Weights;
		}

		for (int i = 0; i < scene->GetMaterialCount(); i++)
		{
			FbxSurfaceMaterial* material = scene->GetMaterial(i);
			string materialName = material->GetName();

			vector<FbxVertex *> gather;
			for (FbxVertex* temp : meshData->Vertex)
			{
				if (temp->MaterialName == materialName)
					gather.push_back(temp);
			}
			if (gather.size() < 1) continue;


			FbxMeshPartData* meshPart = new FbxMeshPartData();
			meshPart->MaterialName = materialName;

			for (FbxVertex* temp : gather)
			{
				VertexTextureNormalBlend vertex;
				vertex = temp->Vertex;

				meshPart->Vertices.push_back(vertex);
				meshPart->Indices.push_back(meshPart->Indices.size());
			}

			//UINT count = 0;
			//for (int i = 0; i < mesh->GetPolygonCount(); i++)
			//{
			//	for (int vi = 0; vi < 3; vi++)
			//	{
			//		meshPart->Vertices[count].normal = Fbx::Utility::GetNormal(mesh, i, vi);
			//		count++;
			//	}
			//}

			meshData->MeshParts.push_back(meshPart);
		}

	}
}

void Fbx::Exporter::WriteMeshData(wstring saveFolder, wstring saveName)
{
	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + saveName + L".mesh");

	w->UInt(boneDatas.size());
	for (FbxBoneData* bone : boneDatas)
	{
		w->Int(bone->Index);
		w->String(bone->Name);

		w->Int(bone->Parent);
		w->Matrix(bone->Transform);
		w->Matrix(bone->AbsoluteTransform);

		SafeDelete(bone);
	}

	w->UInt(meshDatas.size());
	for (FbxMeshData* meshData : meshDatas)
	{
		w->String(meshData->Name);
		w->Int(meshData->ParentBone);

		w->UInt(meshData->MeshParts.size());
		for (FbxMeshPartData* part : meshData->MeshParts)
		{
			w->String(part->MaterialName);

			w->UInt(part->Vertices.size());
			w->Byte(&part->Vertices[0], sizeof(VertexTextureNormalBlend) * part->Vertices.size());

			w->UInt(part->Indices.size());
			w->Byte(&part->Indices[0], sizeof(UINT) * part->Indices.size());

			SafeDelete(part);
		}

		SafeDelete(meshData);
	}

	w->Close();
	SafeDelete(w);
}

UINT Fbx::Exporter::GetBoneIndexByName(string name)
{
	for (size_t i = 0; i < boneDatas.size(); i++)
	{
		if (boneDatas[i]->Name == name)
			return i;
	}

	return -1;
}

void Fbx::Exporter::ReadAnimation()
{
	FbxTime::EMode mode = FbxTime::EMode::eFrames30;//scene->GetGlobalSettings().GetTimeMode();
	double frameRate = (double)FbxTime::GetFrameRate(mode);

	//FbxArray<FbxString *> animationArray;
	//FbxDocument* document = dynamic_cast<FbxDocument *>(scene);
	//if (document != NULL)
	//	document->FillAnimStackNameArray(animationArray);

	for (int i = 0; i < importer->GetAnimStackCount(); i++)
	{
		FbxAnimation* animation = new FbxAnimation();
		animation->FrameRate = (float)frameRate;

		FbxTakeInfo* takeInfo = importer->GetTakeInfo(i);
		animation->Name = takeInfo->mName.Buffer();

		//TimeSpan: 시간 간격
		FbxTimeSpan span = takeInfo->mLocalTimeSpan;
		double start = (double)span.GetStart().GetFrameCount();
		double end = (double)span.GetStop().GetFrameCount();


		if (start < end)
		{

			// KeyFrames의 수 = Animation 실행 시간(초) * 초당 Frmae 수 + 1(프레임 0)
			// frameRate가 30이라는 것은, 초당 30회 애니메이션이라는 뜻이다.
			// 그러므로 실행시간이 1초라면 딱 30프레임에 맞춰지고,
			// 1초가 넘어가서 1.25정도되면 36프레임.. 이런식으로 맞춰진다.
			//animation->TotalFrame = (int)((end - start) * frameRate);


			//실질적인 애니메이션의 키프레임 데이터를 불러온다.
			//animation->TotalFrame = (int)(end - start) + 1;
			ReadAnimation(animation, scene->GetRootNode(), (int)start, (int)end);
		}//if(start)

		animation->TotalFrame = (int)animation->Keyframes[0]->Transforms.size();

		animDatas.push_back(animation);
	}//for(i)
}

void Fbx::Exporter::ReadAnimation(FbxAnimation* animation, FbxNode * node, int start, int end)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	//만약 Joint(Bone)는 없는데, Animation은 있을 경우. Legacy Animation이다.
	//Legacy Animation은 각 애니메이션 정보에 Mesh정보가 들어있다.
	//그래서 이 정보를 가지고 Bone을 만들어줘야 함.

	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();
		if (nodeType == FbxNodeAttribute::eSkeleton)
		{
			FbxKeyframe* keyframe = new FbxKeyframe();
			keyframe->BoneName = node->GetName();

			for (int i = start; i <= end; i++)
			{
				FbxTime animationTime;
				animationTime.SetFrame(i);

				FbxAMatrix matrix = node->EvaluateLocalTransform(animationTime);
				D3DXMATRIX transform = Utility::ToMatrix(matrix);

				FbxKeyframeData data;
				data.Transform = transform;
				data.Translation = D3DXVECTOR3(transform._41, transform._42, transform._43);
				D3DXQuaternionRotationMatrix(&data.Rotation, &transform);

				FbxVector4 scale = node->EvaluateLocalScaling(animationTime);
				data.Scale = Utility::ToVector3(scale);

				keyframe->Transforms.push_back(data);
			}
			animation->Keyframes.push_back(keyframe);

		}//if(nodeType)
	}//if(attribute)


	for (int i = 0; i < node->GetChildCount(); i++)
		ReadAnimation(animation, node->GetChild(i), start, end);
}

void Fbx::Exporter::WriteAnimation(wstring saveFolder, wstring saveName)
{
	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + saveName + L".anim");

	w->UInt(animDatas.size());
	for (FbxAnimation* animation : animDatas)
	{
		w->String(animation->Name);

		w->Int(animation->TotalFrame);
		w->Float(animation->FrameRate);

		w->UInt(animation->Keyframes.size());
		for (FbxKeyframe* frame : animation->Keyframes)
		{
			w->String(frame->BoneName);

			w->UInt(frame->Transforms.size());
			w->Byte(&frame->Transforms[0], sizeof(FbxKeyframeData) * frame->Transforms.size());

			SafeDelete(frame);
		}

		SafeDelete(animation);
	}

	w->Close();
	SafeDelete(w);
}