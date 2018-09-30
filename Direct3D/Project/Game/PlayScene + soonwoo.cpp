
#include "stdafx.h"
#include "PlayScene.h"

#include "../TerrainTool/Terrain.h"
#include "../TerrainTool/Scattering.h"

#include "./View/FreeCamera.h"

#include "./Environment/Ocean.h"

#include "./Renders/Instancing/InstanceRenderer.h"

#include "./Utilities/Transform.h"
#include "./Utilities/ImGuiHelper.h"

#include "./Figure/Figure.h"
#include "./Object/GameUnit/GameUnit.h"

#include "./Bounding/QuadTree/QuadTreeSystem.h"
#include "./Environment/GameMap.h"

void PlayScene::InitSoonwoo()
{
	Scenes->SetIsTool(false);

	FreeCamera* camera = new FreeCamera;
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::System, camera);
	Objects->SetMainCamera(camera);

	QuadTreeSystem* quadTree = new QuadTreeSystem;
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::System, quadTree);

	Scattering* scattering = new Scattering(camera, "level");
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, scattering);

	GameUnit* player = new GameUnit("Pandaren");
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Unit, player);
}

void PlayScene::InitHuynjin()
{
	GameMap* map = new GameMap;
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, map);
}