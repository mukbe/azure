#include "stdafx.h"
#include "Inspector.h"

#include "ToolScene.h"
#include "Hierarchy.h"

#include "./Object/GameObject/GameObject.h"

#include "../TerrainTool/Terrain.h"
#include "../TerrainTool/Scattering.h"

#include "./View/FreeCamera.h"

Inspector::Inspector(ToolScene * toolScene)
	:ToolBase(toolScene)
{
	name = "Inspector";
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
		object->UIUpdate();
}

void Inspector::PostUpdate()
{
}

void Inspector::PreRender()
{
}

void Inspector::Render()
{
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
			ImGui::Separator();
			object->UIRender();
			ImGui::EndGroup();
		}
	}
	ImGui::End();
}
