#pragma once
#define BufferCount 4
class DeferredRenderer
{
	//0 == depth;
	//1 == normal
	//2 == position
	//3 == diffuse
private:
	class RenderTargetBuffer* renderTarget;
	class Shader* shader;
	class OrthoWindow* orthoWindow;
	class ViewProjectionBuffer* viewProj;
	D3DXMATRIX matOrtho;
public:
	DeferredRenderer();
	~DeferredRenderer();

	void SetRenderTarget();
	void ClearRenderTarget();

	void Render();

	class RenderTargetBuffer* GetGbuffer() { return this->renderTarget; }

};

