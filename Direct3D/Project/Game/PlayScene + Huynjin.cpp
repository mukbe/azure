#include "stdafx.h"
#include "PlayScene.h"
#include "./Environment/GameMap.h"

void PlayScene::InitHuynjin()
{

	GameMap* map = new GameMap;
	Objects->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, map);

}