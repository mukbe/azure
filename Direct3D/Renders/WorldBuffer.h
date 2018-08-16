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

	void SetOrtho(D3DXMATRIX  mat)
	{
		Data.ortho = mat;
		D3DXMatrixTranspose(&Data.ortho, &Data.ortho);
	}

	struct Struct
	{
		D3DXMATRIX View;
		D3DXMATRIX Projection;
		D3DXMATRIX ViewProjection;
		D3DXMATRIX InvView;
		D3DXMATRIX InvProjection;
		D3DXMATRIX InvViewProjection;
		D3DXMATRIX ortho;
	};

private:
	Struct Data;

	ShaderBuffer_Mecro(ViewProjectionBuffer)
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

	ShaderBuffer_Mecro(CameraBuffer)
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


class OrthoBuffer : public ShaderBuffer
{
private:
	struct Struct
	{
		D3DXMATRIX ortho;
	}data;
public:
	OrthoBuffer()
		:ShaderBuffer(&data, sizeof Struct) {}

	void SetMatrix(D3DXMATRIX mat)
	{
		data.ortho = mat;
		D3DXMatrixTranspose(&data.ortho, &data.ortho);
	}
};

class TestBuffer : public ShaderBuffer
{
public:
	struct Struct
	{
		D3DXVECTOR4 PerspectiveValues;
		D3DXMATRIX ViewInv;
	}data;
public:
	TestBuffer()
		:ShaderBuffer(&data, sizeof Struct) {}

	void SetMatrix(D3DXMATRIX mat)
	{
		data.ViewInv = mat;
		D3DXMatrixTranspose(&data.ViewInv, &data.ViewInv);
	}

	ShaderBuffer_Mecro(TestBuffer)
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
