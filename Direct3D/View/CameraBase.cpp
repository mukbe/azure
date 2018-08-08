#include "stdafx.h"
#include "CameraBase.h"

#include "../Utilities/Transform.h"
#include "../Renders/WorldBuffer.h"
#include "Perspective.h"

CameraBase::CameraBase()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	this->perspective = new Perspective((float)desc.Width, (float)desc.Height);
	D3DXMatrixLookAtLH(&matView, &transform->GetWorldPosition(), &(transform->GetWorldPosition() + transform->GetForward()), &D3DXVECTOR3(0.f, 1.f, 0.f));

	this->viewProjectionBuffer =new ViewProjectionBuffer;
	this->viewProjectionBuffer->SetView(matView);
	this->viewProjectionBuffer->SetProjection(perspective->GetMatrix());
	this->viewProjectionBuffer->SetVP(matViewProj);

	this->transform = new Transform;
}

CameraBase::~CameraBase()
{
	SAFE_DELETE(perspective);
	SAFE_DELETE(transform);
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
	this->viewProjectionBuffer->SetView(matView);
	this->viewProjectionBuffer->SetProjection(perspective->GetMatrix());
	this->viewProjectionBuffer->SetVP(matViewProj);

	this->viewProjectionBuffer->SetVSBuffer(0);
}
