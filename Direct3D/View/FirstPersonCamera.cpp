#include "stdafx.h"
#include "FirstPersonCamera.h"


#include "Perspective.h"
#include "../Utilities/Transform.h"
#include "../Utilities/Math.h"


FirstPersonCamera::FirstPersonCamera()
{
	nowAngleH = 0.0f;
	nowAngleV = 0.0f;
	maxAngleV = 85.0f;
	minAngleV = -85.0f;
}


FirstPersonCamera::~FirstPersonCamera()
{
}

void FirstPersonCamera::Init()
{
}

void FirstPersonCamera::Release()
{
}

void FirstPersonCamera::Update()
{
	this->ControlCamera();
	this->UpdateView();

	CameraBase::Update();
}

void FirstPersonCamera::Render()
{
	CameraBase::Render();
}

void FirstPersonCamera::ControlCamera()
{
}

void FirstPersonCamera::UpdateView()
{
	D3DXMatrixLookAtLH(&matView, &transform->GetWorldPosition(), &(transform->GetWorldPosition() + transform->GetForward()), &transform->GetUp());
	perspective->SetView(matView);
}
