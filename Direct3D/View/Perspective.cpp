#include "stdafx.h"
#include "Perspective.h"

Perspective::Perspective(float width, float height, float fov, float zn, float zf)
{
	Set(width, height, fov, zn, zf);
	viewProjectionBuffer->SetProjection(projection);
}

Perspective::~Perspective()
{
}

void Perspective::Set(float width, float height, float fov, float zn, float zf)
{
	this->width = width;
	this->height = height;
	this->fov = fov;
	this->aspect = width / height;
	this->zn = zn;
	this->zf = zf;

	D3DXMatrixPerspectiveFovLH(&projection, fov, aspect, zn, zf);
}

void Perspective::Render()
{
	viewProjectionBuffer->SetVSBuffer(0);
	viewProjectionBuffer->SetPSBuffer(0);
	viewProjectionBuffer->SetDSBuffer(0);
	viewProjectionBuffer->SetGSBuffer(0);
	viewProjectionBuffer->SetCSBuffer(0);

}

void Perspective::SetView(D3DXMATRIX v)
{
	viewProjectionBuffer->SetView(v);

	D3DXMatrixMultiply(&matViewProj, &v, &projection);
	viewProjectionBuffer->SetVP(matViewProj);
}
