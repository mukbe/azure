#include "stdafx.h"
#include "DirectionalLight.h"

#include "../Utilities/Transform.h"

DirectionalLight::DirectionalLight()
{
	D3DXVECTOR3 direction(1.f, -1.f, 0.f);
	D3DXVec3Normalize(&direction, &direction);

	D3DXVECTOR3 origin = D3DXVECTOR3(0.f, 0.f, 0.f) - direction * 30.0f;
	D3DXVECTOR3 up;
	D3DXVec3Cross(&up, &D3DXVECTOR3(1.f,0.f,0.f), &direction);

	D3DXMatrixLookAtLH(&view, &origin, &(origin + direction), &up);
	D3DXMatrixOrthoLH(&ortho, WinSizeX, WinSizeY, 0.f, 1000.f);

	this->pos = origin;
}


DirectionalLight::~DirectionalLight()
{
	
}

D3DXMATRIX DirectionalLight::GetView()
{
	return view;
}

D3DXMATRIX DirectionalLight::GetProj()
{
	return ortho;
}

void DirectionalLight::UpdateView()
{
	D3DXVECTOR3 direction = D3DXVECTOR3(0.f, 0.f, 0.f) - pos;
	D3DXVec3Normalize(&direction, &direction);
	D3DXVECTOR3 up;
	D3DXVec3Cross(&up, &D3DXVECTOR3(1.f, 0.f, 0.f), &direction);
	this->dir = direction;
	D3DXMatrixLookAtLH(&view, &this->pos, &(this->pos + direction), &D3DXVECTOR3(0.f,1.f,0.f));
	//D3DXMatrixOrthoLH(&ortho, WinSizeX, WinSizeY, 0.f, 1000.f);

	D3DXVECTOR3 center(0, 0, 0);
	float radius = D3DXVec3Length(&(pos - center));
	float l = center.x - radius;
	float b = center.y - radius;
	float n = center.z - radius;
	float r = center.x + radius;
	float t = center.y + radius;
	float f = center.z + radius;

	D3DXMatrixOrthoOffCenterLH(&this->ortho, l, r, b, t, n, f);

}
