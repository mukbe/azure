#include "stdafx.h"
#include "Hierarchy.h"

#include "ToolScene.h"

#include "./Object/GameObject/GameObject.h"

#include "../TerrainTool/Terrain.h"
#include "../TerrainTool/Scattering.h"

#include "./View/FreeCamera.h"

#include "./Environment/Ocean.h"

Hierarchy::Hierarchy(ToolScene * toolScene)
	:ToolBase(toolScene), targetObject(nullptr)
{
	name = "Hierarchy";

	freeCamera = new FreeCamera;
	Objects->SetMainCamera(freeCamera);
	
	scattering = new Scattering(freeCamera, "level");
	ocean = new Ocean();

	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::View, freeCamera);
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, scattering);
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, ocean);
}

Hierarchy::~Hierarchy()
{
}

void Hierarchy::Init()
{
	inspector = toolScene->GetTool<Inspector>("Inspector");
}

void Hierarchy::Release()
{
}

void Hierarchy::PreUpdate()
{
	
}

void Hierarchy::Update()
{
	
}

void Hierarchy::PostUpdate()
{
	
}

void Hierarchy::PreRender()
{
	
}

void Hierarchy::Render()
{
	
}

void Hierarchy::UIRender()
{
	ImGui::Begin("Hierarchy");
	{
		ObjectManager::ObjectList objectList = ObjectManager::Get()->objectContainer[ObjectType::Type::Dynamic];
		ObjectManager::ObjectListIter iter = objectList.begin();

		for (; iter != objectList.end(); ++iter)
		{
			string category = ObjectManager::Get()->GetTagName(iter->first);
			if (ImGui::TreeNode(category.c_str()))
			{
				ObjectManager::ArrObject list = iter->second;
				for (UINT i = 0; i < list.size(); ++i)
				{
					if (ImGui::Selectable(list[i]->GetName().c_str()))
						targetObject = list[i];
				}
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}
