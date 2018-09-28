#include "stdafx.h"
#include "GameUnit.h"

#include "./Model/Model.h"
#include "./Model/ModelAnimPlayer.h"
#include "../../Bounding/GameCollider.h"
#include "./Bounding/AnimationCollider.h"

GameUnit::GameUnit(string name,class Model* model)
	:GameObject(name),model(model)
{
	this->animation = new ModelAnimPlayer(model);
}

GameUnit::GameUnit(string name)
	:GameObject(name)
{
	ModelData data = AssetManager->GetModelData(name);
	this->model = new Model;
	data.Clone(model);
	model->BindMeshData();

	this->animation = new ModelAnimPlayer(model);
	this->animation->SetTransform(this->transform);

	for (UINT i = 0; i < data.colliders.size(); ++i)
	{
		AnimationCollider* newCollider = nullptr;
		((AnimationCollider*)data.colliders[i])->Clone((void**)&newCollider);
		newCollider->SetParentObject(this);
		newCollider->SetAnimation(this->animation);
		this->colliderList.push_back(newCollider);
	}

	this->transform->SetScale(0.1f, 0.1f, 0.1f);
	this->transform->SetWorldPosition(50.0f, 10.0f, 50.0f);
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

void GameUnit::PreUpdate()
{
}

void GameUnit::Update()
{
	this->animation->Update();

	for (UINT i = 0; i < colliderList.size(); ++i)
		colliderList[i]->Update();
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

	if (ObjectManager::Get()->GetIsDebug())
	{
		for (UINT i = 0; i < colliderList.size(); ++i)
			colliderList[i]->Render();
	}
}

void GameUnit::PostRender()
{
}
