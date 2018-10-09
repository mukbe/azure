#pragma once
#define BUFFER_COUNT 3

class DeferredRenderer : public Renderer
{
//MRT0 Normal.xyz, RenderType(float)
//MRT1 Diffuse.rgb,Depth
//MRT2 Specr.rgb, SpecPower(float)
//MRT4 DepthMap

//RenderType -- 0.0f ~ 0.9f == ����� ��
//RenderType -- 1.0f ~ 1.9f == ����� ����(GBuffer�� �Ѿ�� Diffuse���) 
private:
	//Multi RenderTarget Values -----------------------------
	ID3D11Texture2D * mrtTexture[BUFFER_COUNT];
	ID3D11RenderTargetView* mrtView[BUFFER_COUNT];
	ID3D11ShaderResourceView* mrtSRV[BUFFER_COUNT];

	ID3D11Texture2D* mrtDepthBufferTexture;
	ID3D11ShaderResourceView* mrtDepthBufferSRV;

	ID3D11DepthStencilView* depthStencilView;
	ID3D11DepthStencilState* depthStencilState;
	//-------------------------------------------------------

	class RenderTargetBuffer*		deferredRenderingBuffer;

	D3D11_VIEWPORT			viewport;
	class Shader*			shader;
	class OrthoWindow*		orthoWindow;
	class DepthVis*			depthVis;
	class UnPacker*			unPacker;
	
	class DeferredBuffer* deferredBuffer;
public:
	DeferredRenderer();
	virtual~DeferredRenderer();

	virtual void SetRTV();		//RenderTarget�� ����ְ� ���� ������ ���������� �о��ش�.
	virtual void Render();		//2Dȭ��� GBuffer������� ���� �ۼ�

	void UIRender();
	void SetUnPackInfo(D3DXMATRIX view, D3DXMATRIX projection);
	ID3D11DepthStencilView* GetDepthStencilView()const { return this->depthStencilView; }
	class RenderTargetBuffer* GetRenderTargetBuffer()const { return this->deferredRenderingBuffer; }
	ID3D11ShaderResourceView* GetRenderTargetSRV();
private:
	bool Create();

};

#include "ShaderBuffer.h"

class DeferredBuffer : public ShaderBuffer
{
public:
	struct Struct
	{
		float shadowBias;
		D3DXVECTOR3 padding;
	}data;

	DeferredBuffer()
		:ShaderBuffer(&data, sizeof Struct) {
		data.shadowBias = 0.007f;
	}
};