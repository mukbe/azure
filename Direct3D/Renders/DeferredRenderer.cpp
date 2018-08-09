#include "stdafx.h"
#include "DeferredRenderer.h"

#include "RenderTargetBuffer.h"
#include "OrthoWindow.h"
#include "./Renders/WorldBuffer.h"

DeferredRenderer::DeferredRenderer()
{
	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	renderTarget = new RenderTargetBuffer(desc.Width, desc.Height, DXGI_FORMAT_R8G8B8A8_UNORM);
	renderTarget->SetArraySize(4);
	renderTarget->Create();

	this->shader = new Shader(Shaders + L"003_Texture.hlsl");
	this->orthoWindow = new OrthoWindow(desc.Width, desc.Height);
	this->viewProj = new ViewProjectionBuffer;
	D3DXMatrixOrthoLH(&matOrtho, (float)desc.Width, (float)desc.Height, 0.f, 1000.0f);
	
}


DeferredRenderer::~DeferredRenderer()
{
	
}

void DeferredRenderer::SetRenderTarget()
{
	this->ClearRenderTarget();

	ID3D11RenderTargetView* renderTargetView = renderTarget->GetRTV();
	ID3D11DepthStencilView* depthStencil = renderTarget->GetDSV();
	//DeviceContext->OMSetRenderTargets(4, &renderTargetView, depthStencil);
}

void DeferredRenderer::ClearRenderTarget()
{
	ID3D11DepthStencilView* depthStencil = renderTarget->GetDSV();
	DeviceContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);
	ID3D11RenderTargetView* renderTargetView = renderTarget->GetRTV();
	DeviceContext->ClearRenderTargetView(renderTargetView, D3DXCOLOR(0.49f, 0.49f, 0.49f, 1.0f));
}

void DeferredRenderer::Render()
{
	viewProj->SetProjection(matOrtho);
	viewProj->SetVSBuffer(0);

	orthoWindow->Render();

	ID3D11ShaderResourceView* srv = renderTarget->GetSRV();
	DeviceContext->PSSetShaderResources(0, 4, &srv);

	shader->Render();

	DeviceContext->DrawIndexed(6,0,0);
}
