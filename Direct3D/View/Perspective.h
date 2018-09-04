#pragma once
#include "Projection.h"

class Perspective : public Projection
{
public:
	Perspective(float width, float height, float fov = (float)D3DX_PI * 0.25f, float zn = 0.1f, float zf = 1000.0f);
	~Perspective();

	void Set(float width, float height, float fov = (float)D3DX_PI * 0.25f, float zn = 0.1f, float zf = 1000.0f);
	void Render();

	void SetView(D3DXMATRIX v);
	D3DXMATRIX GetViewProj()const { return this->matViewProj; }
private:
	float width, height;
	float fov;
	float aspect;
	float zn, zf;

};

