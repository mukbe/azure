#pragma once
class AlphaRenderer : public Renderer
{
private:
	ID3D11RenderTargetView*		renderTargetView;
	ID3D11DepthStencilView*		depthView;
public:
	AlphaRenderer();
	~AlphaRenderer();

	void SetDepthStencilView(ID3D11DepthStencilView* depthView) { this->depthView = depthView; }

	virtual void SetRTV();	
	virtual void Render();		

	void UIRender();
};

