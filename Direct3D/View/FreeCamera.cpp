#include "stdafx.h"
#include "FreeCamera.h"

#include "Perspective.h"
#include "../Utilities/Transform.h"
#include "../Utilities/Math.h"


FreeCamera::FreeCamera()
{
	nowAngleH = 0.0f;
	nowAngleV = 0.0f;
	maxAngleV = 85.0f;
	minAngleV = -85.0f;

	nowSpeed = 3.0f;
	maxSpeed = 50.0f;
}


FreeCamera::~FreeCamera()
{
}

void FreeCamera::Init()
{
}

void FreeCamera::Releaes()
{
}

void FreeCamera::Update()
{
	this->ControlCamera();
	this->UpdateView();

	CameraBase::Update();
}

void FreeCamera::Render()
{
	CameraBase::Render();
}

void FreeCamera::UIRender()
{
	D3DXVECTOR3 pos = transform->GetWorldPosition();
	ImGui::Text("Position : %f , %f , %f", pos.x, pos.y, pos.z);
}

void FreeCamera::ControlCamera()
{
	if (Keyboard::Get()->Down(VK_RBUTTON))
	{
		int screenCenterX = WinSizeX / 2;
		int screenCenterY = WinSizeY / 2;

		Mouse::Get()->SetMousePos(screenCenterX, screenCenterY);
	}
	else if (Keyboard::Get()->Press(VK_RBUTTON))
	{
		float speed = 300.0f;
		D3DXVECTOR3 inputVector(0, 0, 0);

		maxSpeed = 50.0f;
		if (KeyCode->Press(VK_LSHIFT))
		{
			maxSpeed = 200.0f;
		}

		if (KeyCode->Press(VK_SPACE))
		{
			maxSpeed = 1000.f;
		}

		if (KeyCode->Press('W'))
			inputVector.z = speed;
		else if (KeyCode->Press('S'))
			inputVector.z = -speed;
		if (KeyCode->Press('A'))
			inputVector.x = -speed;
		else if (KeyCode->Press('D'))
			inputVector.x = speed;
		if (KeyCode->Press('E'))
			inputVector.y = speed;
		else if (KeyCode->Press('Q'))
			inputVector.y = -speed;

		if (VECTORZERO(inputVector) == false)
			D3DXVec3Normalize(&inputVector, &inputVector);

		D3DXVECTOR3 target = inputVector * maxSpeed;
		transform->MovePositionLocal(target * DeltaTime);
		int screenCenterX = WinSizeX / 2;
		int screenCenterY = WinSizeY / 2;

		POINT mousePos;
		GetCursorPos(&mousePos);

		D3DDesc desc;
		DxRenderer::GetDesc(&desc);

		ScreenToClient(desc.Handle, &mousePos);

		float deltaX = (float)mousePos.x - (float)screenCenterX;
		float deltaY = (float)mousePos.y - (float)screenCenterY;

		nowAngleH += deltaX * 0.2f;
		nowAngleV += deltaY * 0.2f;

		nowAngleV = Math::Clamp(nowAngleV, minAngleV, maxAngleV);

		GameMouse->SetMousePos(screenCenterX, screenCenterY);

		transform->RotateWorld(nowAngleV * ONE_RAD, nowAngleH *ONE_RAD, 0.0f);
	}
}

void FreeCamera::UpdateView()
{
	D3DXMatrixLookAtLH(&matView, &transform->GetWorldPosition(), &(transform->GetWorldPosition() + transform->GetForward()), &transform->GetUp());
	perspective->SetView(matView);

}
