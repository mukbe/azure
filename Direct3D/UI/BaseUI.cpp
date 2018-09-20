#include "stdafx.h"
#include "BaseUI.h"

#include "./View/OrthoWindow.h"

BaseUI::BaseUI()
	:position(0.f,0.f),size(1.f,1.f), pParent(nullptr), shader(nullptr)
{
	orthoWindow = new OrthoWindow(position, size);
}

BaseUI::BaseUI(D3DXVECTOR2 pos, D3DXVECTOR2 size)
	: pParent(nullptr),position(pos),size(size), shader(nullptr)
{
	orthoWindow = new OrthoWindow(position, size);
}


BaseUI::~BaseUI()
{
	SafeDelete(orthoWindow);
}

void BaseUI::SetPosition(D3DXVECTOR2 pos)
{
	this->position = pos;
	this->orthoWindow->SetPosition(pos);
	this->orthoWindow->UpdateBuffer();
}

void BaseUI::SetSize(D3DXVECTOR2 size)
{
	this->size = size;
	this->orthoWindow->SetSize(size);
	this->orthoWindow->UpdateBuffer();
}
