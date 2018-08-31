#include "stdafx.h"
#include "Sun.h"
#include "./Renders/WorldBuffer.h"


Environment::Sun::Sun()
{
	this->sunBuffer = Buffers->FindShaderBuffer<SunBuffer>();

	this->UpdateView();
}


Environment::Sun::~Sun()
{
}

void Environment::Sun::UpdateView()
{
	D3DXVECTOR3 origin, dir;
	D3DXVECTOR3 center(50, 0, 50);
	origin = sunBuffer->GetPos();
	dir = center - origin;
	D3DXVec3Normalize(&dir, &dir);
	sunBuffer->SetDirection(dir);

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
