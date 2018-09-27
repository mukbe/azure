
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

#include "./Bounding/QuadTree/QuadTreeSystem.h"

void PlayScene::InitSoonwoo()
{
	Scenes->SetIsTool(false);

	FreeCamera* camera = new FreeCamera;
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::View, camera);
	Objects->SetMainCamera(camera);

	QuadTreeSystem* quadTree = new QuadTreeSystem;
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::View, quadTree);

	Scattering* scattering = new Scattering(camera, "level");
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, scattering);

	InstanceRenderer* fishingBox = new InstanceRenderer("FishingBox");
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Instancing, fishingBox);

	InstanceRenderer* tree1 = new InstanceRenderer("Tree1");
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Instancing, tree1);

	InstanceRenderer* tree2 = new InstanceRenderer("Tree2");
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Instancing, tree2);
}