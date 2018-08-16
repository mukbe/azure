#pragma once
#include "./Renders/ShaderBuffer.h"
class DirectionalLight
{
private:
	D3DXMATRIX shadowMatrix;
	class SunBuffer* sunBuffer;
	class LightViewProj* lightViewBuffer;
	ID3D11SamplerState* shadowSampler;
public:
	DirectionalLight();
	~DirectionalLight();
	
	void UpdateView();
	void SetBuffer();

	D3DXMATRIX view;
	D3DXMATRIX ortho;

};



class LightViewProj : public ShaderBuffer
{
	struct Data
	{
		D3DXMATRIX lightView;
		D3DXMATRIX lightProj;
		D3DXMATRIX lightViewProj;
		D3DXMATRIX shadowMatrix;
	}data;

public:
	LightViewProj()
		:ShaderBuffer(&data, sizeof Data)
	{

	}

	void SetViewProj(D3DXMATRIX mat)
	{
		this->data.lightViewProj = mat;
		D3DXMatrixTranspose(&data.lightViewProj, &data.lightViewProj);
	}

	void SetShadowMatrix(D3DXMATRIX mat)
	{
		this->data.shadowMatrix = mat;
		D3DXMatrixTranspose(&data.shadowMatrix, &data.shadowMatrix);
	}
};

