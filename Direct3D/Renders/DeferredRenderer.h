#pragma once
#define BUFFER_COUNT 4
class DeferredRenderer
{
	//0 == normal
	//1 == diffuse
	//2 == spec
	//3 == world;
private:
	ID3D11Texture2D * renderTargetTexture[BUFFER_COUNT];
	ID3D11RenderTargetView* renderTargetView[BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceView[BUFFER_COUNT];
	ID3D11Texture2D* depthBufferTexture;
	ID3D11ShaderResourceView* depthSRV;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11DepthStencilState* depthStencilState;
	D3D11_VIEWPORT viewport;
	
	class Shader* shader;
	class OrthoWindow* orthoWindow;
private:
	void ClearRenderTarget();
	bool Create();
public:
	DeferredRenderer();
	~DeferredRenderer();

	void BegindDrawToGBuffer();		//RenderTarget을 잡아주고 이전 내용을 검은색으로 밀어준다.

	void Render();					//2D화면상에 GBuffer기반으로 내용 작성
	void UIRender();

};

