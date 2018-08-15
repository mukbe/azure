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

	void BegindDrawToGBuffer();		//RenderTarget�� ����ְ� ���� ������ ���������� �о��ش�.

	void Render();					//2Dȭ��� GBuffer������� ���� �ۼ�
	void UIRender();

};

