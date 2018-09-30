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
	:ToolBase(toolScene), targetObject(nullptr)
{
	name = "Inspector";
	debugTransform = new DebugTransform();
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
	if (targetObject)
	{
		debugTransform->ConnectTransform(targetObject->GetTransform());
		debugTransform->Update();
		targetObject->UIUpdate();
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
	if (targetObject)
	{
		targetObject->DebugRender();
		debugTransform->RenderGizmo();
	}
}

void Inspector::UIRender()
{
	ImGui::Begin("Inspector");
	{
		if (targetObject)
		{
			ImGui::BeginGroup();
			ImGui::Text(targetObject->GetName().c_str());
			ImGui::Checkbox("IsActive", targetObject->GetPIsActive());
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				targetObject->SendMSG(TagMessage("Delete"));
				this->debugTransform->ConnectTransform(nullptr);
				targetObject = nullptr;
			}

			debugTransform->RenderGUI();
			if(targetObject)
				targetObject->UIRender();
			ImGui::EndGroup();
		}
	}
	ImGui::End();
}

void Inspector::SaveData()
{
}

void Inspector::LoadData()
{
}
