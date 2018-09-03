#include "stdafx.h"
#include "CharacterTool.h"

#include "./Model/ModelAnimPlayer.h"
#include "./Model/Model.h"
#include "./Model/ModelBone.h"
#include "./Model/ModelMesh.h"
#include "./Model/ModelAnimClip.h"

#include "./Project/AnimationTool/FBX/Exporter.h"

#include "./Utilities/String.h"
#include "./Utilities/Path.h"

#include "./View/FreeCamera.h"
#include "./Utilities/Transform.h"
#include "./Figure/Figure.h"

#include "./Renders/DeferredRenderer.h"
#include "./Renders/Texture.h"

#include "./Renders/Instancing/InstanceRenderer.h"
#include "./Object/GameObject/GameObject.h"

#include "./Bounding/AnimationCollider.h"
#include "./Utilities/DebugTransform.h"
#include "./Utilities/BinaryFile.h"

#include "./Object/Item/GameItem.h"
#include "./Bounding/QuadTree/QuadTreeNode.h"
#include "./Bounding/QuadTree/QuadTreeSystem.h"

CharacterTool::CharacterTool()
	:animation(nullptr), model(nullptr),debugTransform(nullptr),targetCollider(nullptr), showDemo(false), showTool(false)
	, showBone(false), selectBoneIndex(0), debugControl(false),  tempItemModel(nullptr)
{
	//Fbx::Exporter* exporter = new Fbx::Exporter(L"../_Assets/Test.fbx");
	//exporter->ExportMaterial(Assets, L"Test");
	//exporter->ExportMesh(Assets, L"Test");
	//exporter->ExportAnimation(Assets, L"Attack01");
	//SafeDelete(exporter);

	model = new Model;
	model->ReadMaterial(L"../_Assets/Pandaren/Pandaren.material");
	model->ReadMesh(L"../_Assets/Pandaren/Pandaren.mesh");
	model->ReadAnimation(L"../_Assets/Pandaren/Pandaren.anim");
	animation = new ModelAnimPlayer(model);

	debugTransform = new DebugTransform();
	debugTransform->ConnectTransform(new Transform);

}


CharacterTool::~CharacterTool()
{
	this->ReleaseCollider();
	SafeDelete(debugTransform);
	SafeDelete(animation);
	SafeDelete(model);
}

void CharacterTool::Init()
{
}

void CharacterTool::Release()
{
	
}

void CharacterTool::Update()
{
	animation->Update();

	if (targetCollider)
	{
		if (debugControl)
		{
			this->ControlCollider();
		}
		else
			targetCollider->SetlocalMatrix(debugTransform->GetTransform()->GetFinalMatrix());
	}


	for (UINT i = 0; i < colliderList.size(); ++i)
	{
		if (debugControl && targetCollider && targetCollider == colliderList[i])
			continue;
		colliderList[i]->Update();
	}

}

void CharacterTool::Render()
{
	animation->Render();

	for (UINT i = 0; i < colliderList.size(); ++i)
	{
		if (colliderList[i] == targetCollider)
			targetCollider->Render(ColorWhite);
		else
			colliderList[i]->Render();
	}


	if (targetCollider)
	{
		if (debugControl)
			debugTransform->RenderGizmo();
		else
		{
			Transform transform;
			transform.SetTransform(targetCollider->GetFinalMatrix());
			transform.RenderGizmo();
		}
	}



	if (this->showBone)
	{
		D3DXMATRIX boneMat = animation->GetBoneAnimations()[selectBoneIndex];
		D3DXVECTOR3 bonePos(boneMat._41, boneMat._42, boneMat._43);
		pRenderer->ChangeZBuffer(false);
		GizmoRenderer->WireSphere(bonePos, 10.0f, ColorRed);
		pRenderer->ChangeZBuffer(true);
	}
}




void CharacterTool::SetCamera(FreeCamera * camera)
{
	this->freeCamera = camera;
	debugTransform->SetCamera(freeCamera);
}



void CharacterTool::AttachModel(Model * model)
{
	if (animation == nullptr)
		animation = new ModelAnimPlayer(model);
	else
	{
		animation->Init();
		animation->SetAnimationModel(model);
	}
}



void CharacterTool::AddAnimation(wstring fileName)
{
	if (this->model == nullptr)return;

	if (fileName.length() > 0)
	{
		model->ReadAnimation(fileName);
	}
	else
	{
		function<void(wstring)> func = std::bind(&CharacterTool::AddAnimation, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::AnimationFilter, Assets, func);
	}
}


void CharacterTool::SaveAnimation(wstring fileName)
{
	if (fileName.length() > 0)
	{
		model->SaveAnimationData(fileName);
	}
	else
	{
		function<void(wstring)> func = std::bind(&CharacterTool::SaveAnimation, this, placeholders::_1);
		Path::SaveFileDialog(fileName, Path::AnimationFilter, Assets, func);
	}
}

void CharacterTool::SaveCollider(wstring fileName)
{
	if (fileName.length() > 0)
	{
		BinaryWriter* w = new BinaryWriter();
		w->Open(fileName);
		{
			w->UInt(colliderList.size());
			for (UINT i = 0; i < colliderList.size(); ++i)
				GameCollider::SaveCollider(w, colliderList[i]);
		}
		w->Close();
	}
	else
	{
		function<void(wstring)> func = std::bind(&CharacterTool::SaveCollider, this, placeholders::_1);
		Path::SaveFileDialog(fileName, Path::ColliderFilter, Assets, func);
	}
}

void CharacterTool::LoadCollider(wstring fileName)
{
	if (fileName.length() > 0)
	{
		this->ReleaseCollider();

		BinaryReader* r = new BinaryReader;
		r->Open(fileName);
		{
			UINT size = r->UInt();
			for (UINT i = 0; i < size; ++i)
			{
				AnimationCollider* newCollider = new AnimationCollider(nullptr, this->animation);
				GameCollider::LoadCollider(r, newCollider);
				colliderList.push_back(newCollider);
			}
			colliderList[0]->Update();
			this->ChangeTargetCollider(colliderList[0], false);
		}
		r->Close();
	}
	else
	{
		function<void(wstring)> func = std::bind(&CharacterTool::LoadCollider, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::ColliderFilter, Assets, func);
	}
}





void CharacterTool::ShowBoneData()
{
	vector<ModelBone*> bones = model->Bones();
	for (UINT i = 0; i < bones.size(); ++i)
	{
		ImGui::Text("[%d] : %s", i, String::WStringToString(bones[i]->Name()).c_str());
	}

	ImGui::Separator();

	if (ImGui::Button("OK", ImVec2(50, 20)))
		ImGui::CloseCurrentPopup();
}


void CharacterTool::ReleaseCollider()
{
	for (UINT i = 0; i < colliderList.size(); ++i)
		SafeDelete(colliderList[i]);
	colliderList.clear();

	targetCollider = nullptr;
}




void CharacterTool::ChangeTargetCollider(AnimationCollider * collider, bool isFirst)
{
	if (collider == nullptr)return;
	
	if (isFirst)
	{
		if (debugControl)
		{
			debugTransform->GetTransform()->SetTransform(animation->GetBoneAnimation(collider->GetParentBoneIndex()));
		}
		else
			debugTransform->GetTransform()->SetTransform(IdentityMatrix);
	}
	else
	{
		if (debugControl)
			debugTransform->GetTransform()->SetTransform(collider->GetFinalMatrix());
		else
			debugTransform->GetTransform()->SetTransform(collider->GetlocalMatrix());
	}
	this->targetCollider = collider;
	
}



void CharacterTool::ControlCollider()
{
	debugTransform->Update();
	D3DXMATRIX mFinal = debugTransform->GetTransform()->GetFinalMatrix();
	D3DXMATRIX mAnimation = animation->GetBoneAnimation(targetCollider->GetParentBoneIndex());
	D3DXMATRIX inv;
	D3DXMatrixInverse(&inv, nullptr, &mAnimation);
	D3DXMATRIX mLocal = mFinal * inv;
	targetCollider->SetlocalMatrix(mLocal);
	targetCollider->SetFinalMatrix(mFinal);
}


