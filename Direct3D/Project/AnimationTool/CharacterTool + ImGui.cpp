#include "stdafx.h"
#include "CharacterTool.h"

#include "./Model/ModelAnimPlayer.h"
#include "./Model/Model.h"
#include "./Model/ModelBone.h"
#include "./Model/ModelMesh.h"
#include "./Model/ModelMeshPart.h"
#include "./Model/ModelAnimClip.h"

#include "./Project/AnimationTool/FBX/Exporter.h"

#include "./View/FreeCamera.h"
#include "./Utilities/Transform.h"
#include "./Figure/Figure.h"

#include "./Renders/DeferredRenderer.h"
#include "./Renders/Texture.h"
#include "./Renders/Material.h"
#include "./Renders/Instancing/InstanceRenderer.h"

#include "./Utilities/String.h"
#include "./Utilities/Path.h"
#include "./Utilities/DebugTransform.h"
#include "./Utilities/BinaryFile.h"

#include "./Object/GameObject/GameObject.h"

#include "./Bounding/AnimationCollider.h"
#include "./Bounding/QuadTree/QuadTreeSystem.h"

void CharacterTool::UIRender()
{
	
	//ToolPage
	if (showTool)
	{
		ImGui::Begin("Tool");
		{
		//-------------------------------------------------------------
			{
				if (this->model)
				{
					if (ImGui::CollapsingHeader("Animation"))
						this->RenderAnimationTool();
					ImGui::Separator();
					if (ImGui::CollapsingHeader("Bone"))
						this->RenderBoneTool();
					ImGui::Separator();
					if (ImGui::CollapsingHeader("Collider"))
						this->RenderColliderTool();
					ImGui::Separator();
					if (ImGui::CollapsingHeader("Part"))
						this->RenderPart();
				}
			}


			//BeginPopup -----------------------------------------------
			this->DeleteAnimation();
			this->ReNameAnimation();
			this->AddCollider();
			this->DeleteCollider();
			//----------------------------------------------------------
			ImGui::End();
		}
	}
	//DebugTransform
	if (targetCollider)
	{
		debugTransform->RenderGUI();
		targetCollider->SetLocalMatrix(debugTransform->GetTransform()->GetFinalMatrix());
	}
}



void CharacterTool::RenderAnimationTool()
{
	static string comboStr = String::WStringToString(animation->GetCurrentClip()->Name());
	ImGui::BeginGroup();
	{
		//AnimationInfo ------------------------------------------------------------------------------
		ImGui::Text("CurrentAnimation : %s", String::WStringToString(animation->GetCurrentClip()->Name()).c_str());
		ImGui::Text("TimeFactor : %f", animation->GetKeyFrameFactor());
		static bool play = true;
		if (ImGui::Checkbox("Play", &play))
		{
			if (play)
			{
				animation->Play();
				debugControl = false;
				this->ChangeTargetCollider(targetCollider, false);
			}
			else
			{
				animation->Pause();
				debugControl = true;
				this->ChangeTargetCollider(targetCollider, false);
			}
		}
		//---------------------------------------------------------------------------------------------
		//ImGui::Separator();	
		//AnimationList -------------------------------------------------------------------------------
		if (ImGui::BeginCombo("AnimationList", comboStr.c_str()))
		{
			vector<ModelAnimClip*> clips = model->Clips();
			for (UINT i = 0; i < clips.size(); ++i)
			{
				string clipName = String::WStringToString(clips[i]->Name());
				bool isSelected = false;
				if (ImGui::Selectable(clipName.c_str(), isSelected))
				{
					comboStr = clipName;
					animation->ChangeAnimation(i);
				}
				if (isSelected);
				ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		//-----------------------------------------------------------------------------------------------
		if (ImGui::Button("Add", ImVec2(70, 20)))
			this->AddAnimation();
		ImGui::SameLine();
		if (ImGui::Button("Delete", ImVec2(70, 20)))
			ImGui::OpenPopup("DeleteAnimation");
		ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(70, 20)))
			this->SaveAnimation();
		ImGui::SameLine();
		if (ImGui::Button("ReName", ImVec2(70, 20)))
			ImGui::OpenPopup("ReNameAnimation");
	}
	ImGui::EndGroup();
}

void CharacterTool::RenderBoneTool()
{
	ImGui::BeginGroup();
	{
		ImGui::Checkbox("ShowBone", &showBone);
		ImGui::SliderInt("CurrentBone", &selectBoneIndex, 0, model->BoneCount() - 1);
		ImGui::InputInt("CurrentBone()", &selectBoneIndex);

		static string comboStr = String::WStringToString(model->Bones()[selectBoneIndex]->Name());
		comboStr = String::WStringToString(model->Bones()[selectBoneIndex]->Name());
		if (ImGui::BeginCombo("BoneList", comboStr.c_str()))
		{
			vector<ModelBone*> bones = model->Bones();
			for (UINT i = 0; i < bones.size(); ++i)
			{
				bool isSelected = false;
				string boneName = String::WStringToString(bones[i]->Name());
				if (ImGui::Selectable(boneName.c_str(), isSelected))
				{
					comboStr = boneName;
					this->selectBoneIndex = bones[i]->Index();
				}
				if(isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
	ImGui::EndGroup();
}

void CharacterTool::RenderColliderTool()
{
	ImGui::BeginGroup();
	{
		//BeginCombo ColliderList ---------------------------------------------------------------
		ImGui::Checkbox("ZBuffer", &zBufferOn);

		static string comboStr = "NULL";
		if (targetCollider)comboStr = targetCollider->GetName();
		if (ImGui::BeginCombo("ColliderList", comboStr.c_str()))
		{
			for (UINT i = 0; i < colliderList.size(); ++i)
			{
				bool isSelected = false;
				if (ImGui::Selectable(colliderList[i]->GetName().c_str(), isSelected))
				{
					comboStr = colliderList[i]->GetName();
					this->ChangeTargetCollider(colliderList[i], false);
				}
				if (isSelected);
				ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		// ---------------------------------------------------------------------------------------
		//ImGui::Separator();
		if (ImGui::Button("Add", ImVec2(70, 20)))
			ImGui::OpenPopup("AddCollider");
		ImGui::SameLine();
		if (ImGui::Button("Delete", ImVec2(70, 20)))
			ImGui::OpenPopup("DeleteCollider");
		ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(70, 20)))
			this->SaveCollider();
		ImGui::SameLine();
		if (ImGui::Button("Load", ImVec2(70, 20)))
			this->LoadCollider();
	}
	ImGui::EndGroup();
}

void CharacterTool::RenderPart()
{
	ImGui::BeginGroup();
	{
		vector<ModelMesh*> meshes = model->Meshes();
		for (UINT i = 0; i < meshes.size(); ++i)
		{
			if (ImGui::TreeNode(String::WStringToString(meshes[i]->Name()).c_str()))
			{
				vector<ModelMeshPart*> parts = meshes[i]->GetMeshParts();
				for (UINT i = 0; i < parts.size(); ++i)
				{
					Material* material = parts[i]->GetMaterial();
					if (ImGui::TreeNode(String::WStringToString(material->GetName()).c_str()))
					{
						material->UIRender();
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
		
	}
	ImGui::EndGroup();
}


void CharacterTool::DeleteAnimation()
{
	if (ImGui::BeginPopup("DeleteAnimation"))
	{
		static string selectAnimatioStr = String::WStringToString(animation->GetCurrentClip()->Name());
		if (ImGui::BeginCombo("AnimationList", selectAnimatioStr.c_str()))
		{
			vector<ModelAnimClip*> clips = model->Clips();
			for (UINT i = 0; i < clips.size(); ++i)
			{
				string clipName = String::WStringToString(clips[i]->Name());
				bool isSelected = false;
				if (ImGui::Selectable(clipName.c_str(), isSelected))
					selectAnimatioStr = clipName;
				if (isSelected);
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Separator();

		if (ImGui::Button("Delete"))
		{
			model->DeleteClip(String::StringToWString(selectAnimatioStr));
			animation->ChangeAnimation(0);

			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancle"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}


void CharacterTool::ReNameAnimation()
{
	if (ImGui::BeginPopup("ReNameAnimation"))
	{
		static string selectAnimatioStr = String::WStringToString(animation->GetCurrentClip()->Name());
		static char str[48] = {};

		ImGui::InputText("NewName", str, sizeof(char) * 48);

		if (ImGui::BeginCombo("AnimationList", selectAnimatioStr.c_str()))
		{
			vector<ModelAnimClip*> clips = model->Clips();
			for (UINT i = 0; i < clips.size(); ++i)
			{
				string clipName = String::WStringToString(clips[i]->Name());
				bool isSelected = false;
				if (ImGui::Selectable(clipName.c_str(), isSelected))
					selectAnimatioStr = clipName;
				if (isSelected);
				ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		
		ImGui::Separator();

		if (ImGui::Button("OK"))
		{
			model->Clip(String::StringToWString(selectAnimatioStr))->SetName(String::StringToWString(str));
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancle"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}


void CharacterTool::AddCollider()
{
	if (ImGui::BeginPopup("AddCollider"))
	{
		static int selectIndex = 0;
		static int selectType = 0;
		static char str[48] = {};
		bool isSelected = false;
		ImGui::InputText("BoundingName", str, sizeof(char) * 48);
		ImGui::InputInt("ParentBoneIndex", &selectIndex);

		if (ImGui::BeginCombo("BoundingType", GameCollider::GetTypeName(selectType).c_str()))
		{
			if (ImGui::Selectable("RigidBody", &isSelected))selectType = 0;
			if (ImGui::Selectable("Attack", &isSelected))selectType = 1;
			if (ImGui::Selectable("HeatBox", &isSelected))selectType = 2;

			if (isSelected);
			ImGui::SetItemDefaultFocus();

			ImGui::EndCombo();
		}

		ImGui::Separator();

		if (ImGui::Button("Add", ImVec2(70, 20)))
		{
			AnimationCollider* newBounding = new AnimationCollider(nullptr, animation);
			newBounding->SetName(str);
			newBounding->SetParentBoneIndex(selectIndex);
			newBounding->SetType((GameCollider::ColliderType)selectType);

			this->colliderList.push_back(newBounding);

			this->ChangeTargetCollider(newBounding,true);

			//this->targetCollider = newBounding;
			//this->debugTransform->GetTransform()->SetTransform(IdentityMatrix);

			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancle", ImVec2(70, 20)))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}


void CharacterTool::DeleteCollider()
{
	static UINT selectedCollider = 0;
	if(ImGui::BeginPopup("DeleteCollider"))
	{
		for (UINT i = 0; i < colliderList.size(); ++i)
		{
			if (ImGui::Selectable(colliderList[i]->GetName().c_str()))
			{
				targetCollider = colliderList[i];
				selectedCollider = i;
			}
		}

		if (ImGui::Button("Delete",ImVec2(70,20)))
		{
			if (selectedCollider < 0 || selectedCollider >= colliderList.size()|| colliderList.empty())
				ImGui::CloseCurrentPopup();
			else
			{
				SafeDelete(colliderList[selectedCollider]);
				colliderList.erase(colliderList.begin() + selectedCollider);

				if (colliderList.empty())
					targetCollider = nullptr;
				else
					this->ChangeTargetCollider(colliderList[0], false);

				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancle", ImVec2(70, 20)))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}





void CharacterTool::FindPathMaterial(wstring fileName)
{
	if (fileName.length() > 0)
	{
		tempItemModel->ReadMaterial(fileName);
	}
	else
	{
		function<void(wstring)> func = std::bind(&CharacterTool::FindPathMaterial, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::MaterialFilter,Assets, func);
	}
}

void CharacterTool::FindPathMesh(wstring fileName)
{
	if (fileName.length() > 0)
	{
		tempItemModel->ReadMesh(fileName);
	}
	else
	{
		function<void(wstring)> func = std::bind(&CharacterTool::FindPathMesh, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::MeshFilter, Assets, func);
	}
}

