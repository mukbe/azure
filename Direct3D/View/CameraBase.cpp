#include "stdafx.h"
#include "CameraBase.h"

#include "../Utilities/Transform.h"
#include "../Renders/WorldBuffer.h"
#include "Perspective.h"

CameraBase::CameraBase()
{
	D3DDesc desc;
	DxRenderer::GetDesc(&desc);

	this->perspective = new Perspective((float)desc.Width, (float)desc.Height);

	this->transform = new Transform;
	this->transform->SetWorldPosition(0, 0, -20);

	D3DXMatrixLookAtLH(&matView, &transform->GetWorldPosition(), &(transform->GetWorldPosition() + transform->GetForward()), &D3DXVECTOR3(0.f, 1.f, 0.f));

	perspective->SetView(matView);


}

CameraBase::~CameraBase()
{
	SafeDelete(perspective);
	SafeDelete(transform);
}


void CameraBase::Init()
{
}

void CameraBase::Relase()
{
}

void CameraBase::Update()
{
}

void CameraBase::Render()
{
	perspective->Render();
}
