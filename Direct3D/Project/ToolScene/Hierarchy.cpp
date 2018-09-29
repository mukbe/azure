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

	//freeCamera = new FreeCamera;
	//Objects->SetMainCamera(freeCamera);
	//Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::System, freeCamera);
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
			if (ImGui::TreeNode("Enviroment"))
			{
				if (ImGui::Selectable("Ocean"))
				{
					this->CreateEnviroment("Ocean");
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Selectable("Terrain"))
				{
					this->CreateEnviroment("Terrain");
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Selectable("Scattering"))
				{
					this->CreateEnviroment("Scattering");
					ImGui::CloseCurrentPopup();
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Unit"))
			{
				if (ImGui::Selectable("Pandaren"))
				{
					this->CreateUnit("Pandaren");
					ImGui::CloseCurrentPopup();
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Instancing"))
			{
				if (ImGui::Selectable("InstanceRenderer"))
				{
					this->CreaetInstanceRenderer();
					ImGui::CloseCurrentPopup();
				}
				ImGui::TreePop();
			}
			ImGui::EndPopup();
		}

		ImGui::Separator();

		ObjectManager::ObjectContainer container = ObjectManager::Get()->objectContainer;
		ObjectManager::ObjectContainerIter containerIter = container.begin();
		for (; containerIter != container.end(); ++containerIter)
		{
			string typeName;
			if (containerIter->first == ObjectType::Type::Dynamic)
				typeName = "Dynamic";
			else
				typeName = "Static";

			if (ImGui::TreeNode(typeName.c_str()))
			{
				ObjectManager::ObjectList objectList = containerIter->second;
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

				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}

void Hierarchy::SaveData()
{

}

void Hierarchy::LoadData()
{
}

void Hierarchy::CreateUnit(string name)
{
	GameUnit* newUnit = new GameUnit(name);
	newUnit->GetTransform()->SetWorldPosition(MainCamera->GetTransform()->GetWorldPosition() +
		MainCamera->GetTransform()->GetForward() * 50.0f);
	newUnit->GetTransform()->SetScale(0.3f, 0.3f, 0.3f);
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Unit, newUnit);
}

void Hierarchy::CreateEnviroment(string name)
{
	if (name == "Ocean")
	{
		Ocean* ocean = new Ocean;
		ocean->Init();
		Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, ocean);
	}
	else if (name == "Terrain")
	{
		Terrain* terrain = new Terrain;
		Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, terrain);
	}
	else if (name == "Scattering")
	{
		Scattering* scattering = new Scattering((FreeCamera*)MainCamera, "level");
		Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, scattering);
		Objects->ObjectSortingFront(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, "Scattering");
	}
}

void Hierarchy::CreaetInstanceRenderer(wstring fileName)
{
	if (fileName.length() > 0)
	{
		string key = String::WStringToString(fileName);
		key = Path::GetFileNameWithoutExtension(key) + InstanceRenderer::Renderer;
		InstanceRenderer* renderer = new InstanceRenderer(key, 30);
		Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Instancing, renderer);
	}
	else
	{
		function<void(wstring)> func = bind(&Hierarchy::CreaetInstanceRenderer, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::MaterialFilter, Assets, func);
	}
}



