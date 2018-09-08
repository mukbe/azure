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

	struct Struct
	{
		D3DXMATRIX View;
		D3DXMATRIX Projection;
		D3DXMATRIX ViewProjection;
		D3DXMATRIX InvView;
		D3DXMATRIX InvProjection;
		D3DXMATRIX InvViewProjection;

	};

private:
	Struct Data;


	ShaderBuffer_Mecro(ViewProjectionBuffer)
};

class SunBuffer : public ShaderBuffer
{
public:
	SunBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.SunPosition = D3DXVECTOR3(-10.0f, 30.0f, 50.f);
		Data.SunIntensity = 1.0f;
		Data.SunDir = D3DXVECTOR3(1, -1, 0);
		Data.SunColor = D3DXCOLOR(1, 1, 1, 1);

		D3DXVec3Normalize(&Data.SunDir, &Data.SunDir);
	}
	struct Struct
	{
		D3DXMATRIX SunView;

		D3DXVECTOR3 SunPosition;
		float SunPadding;

		float SunIntensity;
		D3DXVECTOR3 SunDir;

		D3DXCOLOR SunColor;

		D3DXMATRIX SunProjection;
		D3DXMATRIX SunViewProjection;
		D3DXMATRIX ShadowMatrix;

	};

	void SetDirection(D3DXVECTOR3 pos) { this->Data.SunDir = pos; }
	void SetColor(D3DXCOLOR col) { this->Data.SunColor = col; }
	void SetIntensity(float i) { this->Data.SunIntensity = i; }
	void SetPos(D3DXVECTOR3 pos) { this->Data.SunPosition = pos; }

	D3DXVECTOR3 GetPos() { return this->Data.SunPosition; }
	D3DXVECTOR3 GetDir() { return this->Data.SunDir; }

	float* GetPosPtr() { return &Data.SunPosition.x; }

	void SetView(D3DXMATRIX mat)
	{
		this->Data.SunView = mat;
		D3DXMatrixTranspose(&Data.SunView, &Data.SunView);
	}

	void SetProj(D3DXMATRIX mat)
	{
		this->Data.SunProjection = mat;
		D3DXMatrixTranspose(&Data.SunProjection, &Data.SunProjection);
	}

	void SetViewProj(D3DXMATRIX mat)
	{
		this->Data.SunViewProjection = mat;
		D3DXMatrixTranspose(&Data.SunViewProjection, &Data.SunViewProjection);
	}

	void SetShadowMatrix(D3DXMATRIX mat)
	{
		this->Data.ShadowMatrix = mat;
		D3DXMatrixTranspose(&Data.ShadowMatrix, &Data.ShadowMatrix);
	}

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
		Data.Ambient = D3DXCOLOR(1, 1, 1, 1);
		Data.Diffuse = D3DXCOLOR(1, 1, 1, 1);
		Data.Specular = D3DXCOLOR(1, 1, 1, 1);
		Data.Emissive = D3DXCOLOR(0, 0, 0, 1);

		Data.Shininess = 1;
		Data.DetailFactor = 1;
	}

	struct Struct
	{
		D3DXCOLOR Ambient;
		D3DXCOLOR Diffuse;
		D3DXCOLOR Specular;
		D3DXCOLOR Emissive;

		float Shininess;
		float DetailFactor;

		float Padding[2];
	}Data;

	ShaderBuffer_Mecro(MaterialBuffer)
};

class UnPacker : public ShaderBuffer
{
public:
	UnPacker()
		: ShaderBuffer(&Data, sizeof(Data))
	{
		D3DXMatrixIdentity(&Data.InvView);
		Data.PerspectiveValues = D3DXVECTOR4(0, 0, 0, 0);
	}

	void SetPerspectiveValues(D3DXMATRIX projection)
	{
		Data.PerspectiveValues.x = 1.0f / projection.m[0][0];
		Data.PerspectiveValues.y = 1.0f / projection.m[1][1];
		Data.PerspectiveValues.z = projection.m[3][2];
		Data.PerspectiveValues.w = -projection.m[2][2];
	}
	
	//view∏¶ ≥÷¿∏∏È invView∑Œ πŸ≤„¡‹
	void SetInvView(D3DXMATRIX view)
	{
		Data.InvView = view;
		D3DXMatrixInverse(&Data.InvView, nullptr, &Data.InvView);
		D3DXMatrixTranspose(&Data.InvView, &Data.InvView);
	}

	struct Struct
	{
		D3DXMATRIX InvView;
		D3DXVECTOR4 PerspectiveValues;
	}Data;
};
