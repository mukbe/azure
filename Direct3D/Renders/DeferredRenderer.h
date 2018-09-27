#pragma once
#define BUFFER_COUNT 3

class DeferredRenderer : public Renderer
{
//MRT0 Normal.xyz, RenderType(float)
//MRT1 Diffuse.rgb,Depth
//MRT2 Specr.rgb, SpecPower(float)
//MRT4 DepthMap

//RenderType -- 0.0f ~ 0.9f == 빛계산 함
//RenderType -- 1.0f ~ 1.9f == 빛계산 안함(GBuffer로 넘어온 Diffuse출력) 
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

	virtual void SetRTV();		//RenderTarget을 잡아주고 이전 내용을 검은색으로 밀어준다.
	virtual void Render();		//2D화면상에 GBuffer기반으로 내용 작성

	void UIRender();
	void SetUnPackInfo(D3DXMATRIX view, D3DXMATRIX projection);
	ID3D11DepthStencilView* GetDepthStencilView()const { return this->depthStencilView; }
private:
	bool Create();

};

