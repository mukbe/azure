#pragma once
#include "./Renders/ShaderBuffer.h"

#pragma once

class WorldBuffer : public ShaderBuffer
{
public:
	WorldBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		D3DXMatrixIdentity(&Data.World);
	}

	void SetMatrix(D3DXMATRIX mat)
	{
		Data.World = mat;
		D3DXMatrixTranspose(&Data.World, &Data.World);
	}

	struct Struct
	{
		D3DXMATRIX World;
	};

private:
	Struct Data;
};

class ViewProjectionBuffer : public ShaderBuffer
{
public:
	ViewProjectionBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		D3DXMatrixIdentity(&Data.View);
		D3DXMatrixIdentity(&Data.Projection);
	}

	void SetView(D3DXMATRIX mat)
	{
		Data.View = mat;
		D3DXMatrixTranspose(&Data.View, &Data.View);
	}

	void SetProjection(D3DXMATRIX mat)
	{
		Data.Projection = mat;
		D3DXMatrixTranspose(&Data.Projection, &Data.Projection);
	}
	void SetVP(D3DXMATRIX mat)
	{
		Data.ViewProjection = mat;
		D3DXMatrixTranspose(&Data.ViewProjection, &Data.ViewProjection);
	}

	struct Struct
	{
		D3DXMATRIX View;
		D3DXMATRIX Projection;
		D3DXMATRIX ViewProjection;
	};

private:
	Struct Data;
};

class CameraBuffer : public ShaderBuffer
{
public:
	CameraBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.cameraPos = D3DXVECTOR3(0.f, 0.f, 0.f);
	}

	struct Struct
	{
		D3DXVECTOR3 cameraPos;
		float padding;
	};

	void SetCameraPosition(D3DXVECTOR3 pos) { this->Data.cameraPos = pos; }

private:
	Struct Data;
};

class SunBuffer : public ShaderBuffer
{
public:
	SunBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.direction = D3DXVECTOR3(1, -1, 0);
		Data.color = D3DXCOLOR(1, 1, 1, 1);
	}

	struct Struct
	{
		D3DXVECTOR3 direction;
		float padding;

		D3DXCOLOR color;

	};

	void SetDirection(D3DXVECTOR3 pos) { this->Data.direction = pos; }
	void SetColor(D3DXCOLOR col) { this->Data.color = col; }
private:
	Struct Data;
};
