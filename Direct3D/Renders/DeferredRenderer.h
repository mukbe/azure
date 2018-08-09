#pragma once
#define BUFFER_COUNT 4
class DeferredRenderer
{
	//0 == normal
	//1 == position
	//2 == diffuse
	//3 == depth;
private:
	ID3D11Texture2D * m_renderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* m_renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* m_shaderResourceViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* depthSRV;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
	
	class Shader* shader;
	class OrthoWindow* orthoWindow;
	class ViewProjectionBuffer* viewProj;
	D3DXMATRIX matOrtho;
public:
	DeferredRenderer();
	~DeferredRenderer();

	void SetRenderTarget();
	void ClearRenderTarget();

	bool Create();

	void Render();
	void PostRender();

};

