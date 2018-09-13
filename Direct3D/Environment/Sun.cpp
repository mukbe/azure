#include "stdafx.h"
#include "Sun.h"
#include "./Renders/WorldBuffer.h"


Environment::Sun::Sun()
	:bUpdate(true),height(5)
{
	this->sunBuffer = Buffers->FindShaderBuffer<SunBuffer>();

	dir = D3DXVECTOR3(0, 0, 1);
	angle = D3DXVECTOR3(0, 0, 0);
	right = D3DXVECTOR3(1, 0, 0);
	preMousePos = D3DXVECTOR3();

	this->UpdateView();
}


Environment::Sun::~Sun()
{
}

void Environment::Sun::Update()
{
	if (bUpdate == false) return;

	//TODO 시간하고 같이 흘러가도록 조정

	if (Mouse::Get()->Down(0) && Keyboard::Get()->Press(VK_SHIFT))
		preMousePos = Mouse::Get()->GetPosition();

	D3DXVECTOR3 current = Mouse::Get()->GetPosition();
	D3DXVECTOR3 delta = current - preMousePos;
	preMousePos = current;

	if (Mouse::Get()->Press(0) && Keyboard::Get()->Press(VK_SHIFT))
	{
		angle.y += delta.x * 0.005f;
		angle.x -= delta.y * 0.005f;


		D3DXMATRIX mat;
		D3DXQUATERNION q;

		D3DXMatrixRotationAxis(&mat, &right, angle.x);
		D3DXVec3TransformNormal(&dir, &dir, &mat);


		D3DXQuaternionRotationYawPitchRoll(&q, angle.y, 0, 0);
		D3DXMatrixRotationQuaternion(&mat, &q);
		D3DXVec3TransformNormal(&dir, &dir, &mat);

		D3DXVec3TransformNormal(&right, &right, &mat);



		angle = D3DXVECTOR3(0,0,0);
		//transform->RotateWorld(0, delta.x * 0.001f, 0);
		//transform->RotateSelf(-delta.y * 0.001f, 0, 0);
	}
}

void Environment::Sun::UpdateView()
{
	//TODO 보는 방향 조정해야됨
	D3DXVECTOR3 center(0, 0, 0), origin;
	D3DXVec3Normalize(&dir, &dir);
	sunBuffer->SetDirection(dir);
	origin = -dir * 50.f;

	D3DXVECTOR3 up;
	D3DXVec3Cross(&up, &D3DXVECTOR3(1.f, 0.f, 0.f), &dir);

	D3DXMatrixLookAtLH(&view, &origin, &(origin + dir), &D3DXVECTOR3(0.f, 1.f, 0.f));

	float radius = D3DXVec3Length(&(origin - center));
	float l = center.x - radius;
	float b = center.y - radius;
	float n = center.z - radius;
	float r = center.x + radius;
	float t = center.y + radius;
	float f = center.z + radius;

	D3DXMatrixOrthoOffCenterLH(&this->ortho, l, r, b, t, n, f);

	D3DXMATRIX viewProj;
	D3DXMatrixMultiply(&viewProj, &view, &ortho);

	//set special texture matrix for shadow mapping
	float fOffsetX = 0.5f + (0.5f / (float)WinSizeX);
	float fOffsetY = 0.5f + (0.5f / (float)WinSizeY);

	D3DXMATRIX toViewport = D3DXMATRIX(0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		fOffsetX, fOffsetY, 0.0f, 1.0f);

	D3DXMatrixMultiply(&shadowMatrix, &viewProj, &toViewport);


	sunBuffer->SetView(view);
	sunBuffer->SetProj(ortho);
	sunBuffer->SetViewProj(viewProj);
	sunBuffer->SetShadowMatrix(shadowMatrix);
}

void Environment::Sun::Render()
{
	sunBuffer->SetPSBuffer(4);
}

float * Environment::Sun::GetPosPtr()
{
	return sunBuffer->GetPosPtr();
}
