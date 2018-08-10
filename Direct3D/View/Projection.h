#pragma once
#include "stdafx.h"
#include "./Renders/WorldBuffer.h"

class Projection
{
public:
	Projection() 
	{
		this->viewProjectionBuffer = new ViewProjectionBuffer;
	}
	~Projection() {}

	virtual void Render() = 0;

	virtual D3DXMATRIX GetMatrix() { return projection; }
	void GetMatrix(D3DXMATRIX* mat)
	{
		memcpy(mat, &projection, sizeof(D3DXMATRIX));
	}
	D3DXMATRIX GetMatrix()const { return this->projection; }

protected :
	D3DXMATRIX projection;
	D3DXMATRIX matViewProj;
	ViewProjectionBuffer* viewProjectionBuffer;

};

