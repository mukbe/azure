#pragma once
#include <functional>

class ShadowRenderer : public Renderer
{
private:
	ID3D11Texture2D *							directionalTexture2D;
	ID3D11DepthStencilView*						directionalDSV;
	ID3D11ShaderResourceView*					directionalSRV;

	function<void(void)>						renderFunc;
	D3D11_VIEWPORT								viewport;

	ID3D11SamplerState*							shadowSampler;
public:
	ShadowRenderer();
	virtual~ShadowRenderer();

	void SetRenderFunc(function<void()> func) { this->renderFunc = func; }

	virtual void SetRTV();
	virtual void Render();

	void UIRender();
	ID3D11ShaderResourceView* GetDirectionalSRV()const { return this->directionalSRV; }

private:
	void CreateDirectionalRenderer();
};

