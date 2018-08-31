#include "stdafx.h"
#include "GameUnit.h"

#include "./Model/Model.h"
#include "./Model/ModelAnimPlayer.h"
#include "../../Bounding/GameCollider.h"
#include "./Object/Item/GameItem.h"

GameUnit::GameUnit(string name,class Model* model)
	:GameObject(name),model(model)
{
	this->animation = new ModelAnimPlayer(model);
}


GameUnit::~GameUnit()
{
	SafeDelete(model);
	SafeDelete(animation);

	for (UINT i = 0; i < colliderList.size(); ++i)
		SafeDelete(colliderList[i]);
	colliderList.clear();
}

void GameUnit::Init()
{
}

void GameUnit::Release()
{
}

void GameUnit::PrevUpdate()
{
}

void GameUnit::Update()
{
	this->animation->Update();

	for (UINT i = 0; i < colliderList.size(); ++i)
		colliderList[i]->Update();

	for (UINT i = 0; i < itemList.size(); ++i)
		itemList[i]->Update();
}

void GameUnit::PostUpdate()
{
}

void GameUnit::PrevRender()
{
}

void GameUnit::Render()
{
	this->animation->Render();

	for (UINT i = 0; i < itemList.size(); ++i)
		itemList[i]->Render();
}

void GameUnit::PostRender()
{
}
