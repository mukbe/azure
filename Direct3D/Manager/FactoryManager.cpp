#include "stdafx.h"
#include "FactoryManager.h"

#include "./Environment/Ocean.h"

#include "./Project/TerrainTool/Scattering.h"
#include "./Environment/GameMap.h"

#include "./Object/GameUnit/GameUnit.h"

#include "./Renders/Instancing/InstanceRenderer.h"
#include "./Renders/GrassRenderer/GrassRenderer.h"

#include "./View/FreeCamera.h"

SingletonCpp(FactoryManager)

FactoryManager::FactoryManager()
{
}


FactoryManager::~FactoryManager()
{
}

void FactoryManager::Create(string name, Json::Value value)
{
	if (name == "Camera")
	{
		//FreeCamera* freeCamera = new FreeCamera;
		//freeCamera->LoadData(&value);
		//Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::System, freeCamera);
		//Objects->SetMainCamera(freeCamera);
		//Objects->ObjectSortingFront(ObjectType::Type::Dynamic, ObjectType::Tag::System, "Camera");
	}
	else if (name == "Ocean")
	{
		Ocean* ocean = new Ocean;
		ocean->LoadData(&value);
		Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, ocean);
	}
	else if (name == "Scattering")
	{
		Scattering* scattering = new Scattering((FreeCamera*)MainCamera, "level");
		scattering->LoadData(&value);
		Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, scattering);
		Objects->ObjectSortingFront(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, "Scattering");
	}
	else if (name =="Terrain")
	{
		string fileName;
		JsonHelper::GetValue(value, "FileName", fileName);

		GameMap* terrain = new GameMap(fileName);
		terrain->LoadData(&value);
		Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment,terrain);
	}
	else if (name == "Pandaren")
	{
		GameUnit* unit = new GameUnit(name);
		unit->LoadData(&value);
		Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Unit, unit);
	}
	else if (name == "GrassRenderer")
	{
		GrassRenderer* grassRenderer = (GrassRenderer*)Objects->FindObject(ObjectType::Type::Dynamic, ObjectType::Tag::Instancing, "GrassRenderer");
		grassRenderer->LoadData(&value);
	}

	if (strstr(name.c_str(), InstanceRenderer::Renderer.c_str()))
		this->CreateInstanceRenderer(name, value);
}

void FactoryManager::CreateInstanceRenderer(string name, Json::Value value)
{
	InstanceRenderer* renderer = new InstanceRenderer(name, &value);
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Instancing, renderer);

}
