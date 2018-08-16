#pragma once
#include "./Renders/ShaderBuffer.h"

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

	ShaderBuffer_Mecro(WorldBuffer)
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
		D3DXMatrixInverse(&Data.InvView, NULL, &Data.View);
		D3DXMatrixTranspose(&Data.InvView, &Data.InvView);
		D3DXMatrixTranspose(&Data.View, &Data.View);
	}

	void SetProjection(D3DXMATRIX mat)
	{
		Data.Projection = mat;
		SetPerspectiveValues();
		D3DXMatrixInverse(&Data.InvProjection, NULL, &Data.Projection);
		D3DXMatrixTranspose(&Data.InvProjection, &Data.InvProjection);
		D3DXMatrixTranspose(&Data.Projection, &Data.Projection);
	}
	void SetVP(D3DXMATRIX mat)
	{
		Data.ViewProjection = mat;
		D3DXMatrixInverse(&Data.InvViewProjection, NULL, &Data.ViewProjection);
		D3DXMatrixTranspose(&Data.InvViewProjection, &Data.InvViewProjection);
		D3DXMatrixTranspose(&Data.ViewProjection, &Data.ViewProjection);
	}


	struct Struct
	{
		D3DXMATRIX View;
		D3DXMATRIX Projection;
		D3DXMATRIX ViewProjection;
		D3DXMATRIX InvView;
		D3DXMATRIX InvProjection;
		D3DXMATRIX InvViewProjection;

		D3DXVECTOR4 PerspectiveValues;
	};

private:
	Struct Data;

	void SetPerspectiveValues()
	{
		Data.PerspectiveValues.x = 1.0f / Data.Projection.m[0][0];
		Data.PerspectiveValues.y = 1.0f / Data.Projection.m[1][1];
		Data.PerspectiveValues.z = Data.Projection.m[3][2];
		Data.PerspectiveValues.w = -Data.Projection.m[2][2];
	}

	ShaderBuffer_Mecro(ViewProjectionBuffer)
};

class SunBuffer : public ShaderBuffer
{
public:
	SunBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.pos = D3DXVECTOR3(-10.0f, 30.0f, 50.f);
		Data.intensity = 1.0f;
		Data.dir = D3DXVECTOR3(1, -1, 0);
		Data.color = D3DXCOLOR(1, 1, 1, 1);

		D3DXVec3Normalize(&Data.dir, &Data.dir);
	}
	struct Struct
	{
		D3DXVECTOR3 pos;
		float intensity;
		D3DXVECTOR3 dir;
		float padding;
		D3DXCOLOR color;

	};

	void SetDirection(D3DXVECTOR3 pos) { this->Data.dir= pos; }
	void SetColor(D3DXCOLOR col) { this->Data.color = col; }
	void SetIntensity(float i) { this->Data.intensity = i; }
	void SetPos(D3DXVECTOR3 pos) { this->Data.pos = pos; }

	D3DXVECTOR3 GetPos() { return this->Data.pos; }
	D3DXVECTOR3 GetDir() { return this->Data.dir; }
private:
	Struct Data;

	ShaderBuffer_Mecro(SunBuffer)
};

class MaterialBuffer : public ShaderBuffer
{
public:
	MaterialBuffer()
		: ShaderBuffer(&Data, sizeof(Data))
	{
		Data.Diffuse = D3DXCOLOR(1, 1, 1, 1);
		Data.Specular = D3DXCOLOR(1, 1, 1, 1);
		Data.Emissive = D3DXCOLOR(0, 0, 0, 1);

		Data.Shininess = 1;
		Data.DetailFactor = 1;
	}

	struct Struct
	{
		D3DXCOLOR Diffuse;
		D3DXCOLOR Specular;
		D3DXCOLOR Emissive;

		float Shininess;
		float DetailFactor;

		float Padding[2];
	}Data;

	ShaderBuffer_Mecro(MaterialBuffer)
};
