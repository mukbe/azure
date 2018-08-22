#include "stdafx.h"
#include "GameItem.h"

#include "./Model/Model.h"
#include "./Renders/WorldBuffer.h"

GameItem::GameItem(Model* model, D3DXMATRIX* pParentMat)
	:model(model),pParentMat(pParentMat)
{
	this->worldBuffer = Buffers->FindShaderBuffer<WorldBuffer>();
	this->model->CopyAbsoluteBoneTo(absolutes);

}


GameItem::~GameItem()
{
	absolutes.clear();
}

void GameItem::Update()
{
}

void GameItem::Render()
{
}
