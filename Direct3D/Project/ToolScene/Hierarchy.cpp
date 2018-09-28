#include "stdafx.h"
#include "Hierarchy.h"

#include "ToolScene.h"
#include "Inspector.h"

#include "./Object/GameUnit/GameUnit.h"
#include "./Object/GameObject/GameObject.h"

#include "../TerrainTool/Terrain.h"
#include "../TerrainTool/Scattering.h"

#include "./View/FreeCamera.h"

#include "./Environment/Ocean.h"

#include "./Renders/Instancing/InstanceRenderer.h"


Hierarchy::Hierarchy(ToolScene * toolScene)
	:ToolBase(toolScene)
{
	name = "Hierarchy";

	freeCamera = new FreeCamera;
	//freeCamera->GetTransform()->SetWorldPosition(278.f, 22.f, 566.f);
	//freeCamera->GetTransform()->RotateSelf(0.f, 180.f * ONE_RAD, 0.f);
	Objects->SetMainCamera(freeCamera);

	scattering = new Scattering(freeCamera, "level");
	ocean = new Ocean();

	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::View, freeCamera);
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, scattering);
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, ocean);
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, new Terrain);

	InstanceRenderer* fishingBox = new InstanceRenderer("FishingBox", 20);
	fishingBox->InitializeData("FishingBox");
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Instancing, fishingBox);

	InstanceRenderer* tree1 = new InstanceRenderer("Tree1", 20);
	tree1->InitializeData("Tree1");
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Instancing, tree1);

	InstanceRenderer* tree2 = new InstanceRenderer("Tree2", 20);
	tree2->InitializeData("Tree2");
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Instancing, tree2);
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
		if (ImGui::Button("Create", ImVec2(100, 20)))
			ImGui::OpenPopup("CreateList");

		if (ImGui::BeginPopup("CreateList"))
		{
			if (ImGui::TreeNode("Unit"))
			{
				if (ImGui::Selectable("Pandaren"))
				{
					this->CreateUnit("Pandaren");
					ImGui::CloseCurrentPopup();
				}
				ImGui::TreePop();
			}

			ImGui::EndPopup();
		}

		ImGui::Separator();
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
					{
						inspector->SetTargetObject(list[i]);
					}
				}
				ImGui::TreePop();

			}
		}
	}
	ImGui::End();
}

void Hierarchy::CreateUnit(string name)
{
	GameUnit* newUnit = new GameUnit(name);
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Unit, newUnit);
}



