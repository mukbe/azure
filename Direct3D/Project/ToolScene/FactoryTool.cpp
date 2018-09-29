#include "stdafx.h"
#include "FactoryTool.h"


FactoryTool::FactoryTool(ToolScene * toolScene)
	:ToolBase(toolScene)
{
	name = "FactoryTool";
}

FactoryTool::~FactoryTool()
{
}

void FactoryTool::Init()
{
}

void FactoryTool::Release()
{
}

void FactoryTool::PreUpdate()
{
}

void FactoryTool::Update()
{
}

void FactoryTool::PostUpdate()
{
}

void FactoryTool::Render()
{
}

void FactoryTool::PreRender()
{
}

void FactoryTool::UIRender()
{
	ImGui::Begin("FactoryTool");
	{
		if (ImGui::Button("Create",ImVec2(100,20)))
			ImGui::OpenPopup("CreateObject");

		if (ImGui::BeginPopupModal("CreateObject"))
		{


			if (ImGui::Button("Close",ImVec2(100,20)))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void FactoryTool::SaveData()
{
}

void FactoryTool::LoadData()
{
}
