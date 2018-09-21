#include "stdafx.h"
#include "Inspector.h"

#include "ToolScene.h"
#include "Hierarchy.h"

#include "./Object/GameObject/GameObject.h"
#include "./Object/GameObject/TagMessage.h"

#include "../TerrainTool/Terrain.h"
#include "../TerrainTool/Scattering.h"

#include "./Utilities/DebugTransform.h"

#include "./View/FreeCamera.h"

Inspector::Inspector(ToolScene * toolScene)
	:ToolBase(toolScene)
{
	name = "Inspector";
	debugTransform = new DebugTransform();
	debugTransform->SetCamera(MainCamera);
}

Inspector::~Inspector()
{
}

void Inspector::Init()
{
	hierarchy = toolScene->GetTool<Hierarchy>("Hierarchy");
}

void Inspector::Release()
{
}

void Inspector::PreUpdate()
{
}

void Inspector::Update()
{
	GameObject* object = hierarchy->GetTargetObject();
	if (object)
	{
		debugTransform->ConnectTransform(object->GetTransform());
		debugTransform->Update();

		object->UIUpdate();
	}
}

void Inspector::PostUpdate()
{
}

void Inspector::PreRender()
{
}

void Inspector::Render()
{
	GameObject* object = hierarchy->GetTargetObject();
	if (object)
	{
		object->DebugRender();
		debugTransform->RenderGizmo();
	}
}

void Inspector::UIRender()
{
	ImGui::Begin("Inspector");
	{
		GameObject* object = hierarchy->GetTargetObject();
		if (object)
		{
			ImGui::BeginGroup();
			ImGui::Text(object->GetName().c_str());
			ImGui::Checkbox("IsActive", object->GetPIsActive());
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				object->SendMSG(TagMessage("Delete"));
				this->debugTransform->ConnectTransform(nullptr);
				this->hierarchy->SetNullTarget();
			
			}
			ImGui::Separator();
			object->UIRender();
			ImGui::EndGroup();
		}
	}
	ImGui::End();
}
