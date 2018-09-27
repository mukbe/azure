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
	ID3D11Texture2D * renderTargetTexture[BUFFER_COUNT];
	ID3D11RenderTargetView* renderTargetView[BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceView[BUFFER_COUNT];

	ID3D11Texture2D* renderAlphaTexture;
	ID3D11RenderTargetView* renderAlphaTargetView;
	ID3D11ShaderResourceView* renderAlphaSRV;

	ID3D11Texture2D* depthBufferTexture;
	ID3D11ShaderResourceView* depthSRV;

	ID3D11DepthStencilView* depthStencilView;
	ID3D11DepthStencilState* depthStencilState;
	D3D11_VIEWPORT viewport;
	
	class Shader*			shader;
	class Shader*			alphaRender;
	class OrthoWindow*		orthoWindow;
	class DepthVis*			depthVis;
	class UnPacker*			unPacker;
public:
	DeferredRenderer();
	virtual~DeferredRenderer();

	virtual void SetRTV();		//RenderTarget�� ����ְ� ���� ������ ���������� �о��ش�.
	virtual void Render();		//2Dȭ��� GBuffer������� ���� �ۼ�

	void UIRender();
	void SetUnPackInfo(D3DXMATRIX view, D3DXMATRIX projection);
	ID3D11DepthStencilView* GetDepthStencilView()const { return this->depthStencilView; }
private:
	bool Create();

};

