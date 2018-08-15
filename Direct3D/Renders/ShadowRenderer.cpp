#include "stdafx.h"
#include "ShadowRenderer.h"

#include "../Utilities/Buffer.h"
#include "WorldBuffer.h"

ShadowRenderer::ShadowRenderer()
	:pointTexture2D(NULL),pointDSV(NULL),pointSRV(NULL),directionalTexture2D(NULL),directionalDSV(NULL),directionalSRV(NULL)
	,renderFunc(NULL), viewProjectionBuffer(NULL)
{
	pair<D3DXMATRIX, D3DXMATRIX> p(IdentityMatrix, IdentityMatrix);
	this->viewProjectionDataContext.assign(6, p);

	this->CreateDirectionalRenderer();

	this->viewProjectionBuffer = new ViewProjectionBuffer;

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(WinSizeX);
	viewport.Height = static_cast<float>(WinSizeY);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}


ShadowRenderer::~ShadowRenderer()
{
	SafeDelete(viewProjectionBuffer);

	SafeRelease(this->pointSRV);
	SafeRelease(this->pointDSV);
	SafeRelease(this->pointTexture2D);

	SafeRelease(this->directionalSRV);
	SafeRelease(this->directionalDSV);
	SafeRelease(this->directionalTexture2D);

	viewProjectionDataContext.clear();
}

void ShadowRenderer::RenderDirectionalMap()
{
	DeviceContext->RSSetViewports(1, &viewport);

	DeviceContext->ClearDepthStencilView(directionalDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	ID3D11RenderTargetView* nullRenderTarget = NULL;
	DeviceContext->OMSetRenderTargets(1, &nullRenderTarget,directionalDSV);
	//this->UpdateViewProjection(0);

	if (this->renderFunc != NULL)
		this->renderFunc();
}

void ShadowRenderer::RenderPointMap()
{
	
}

void ShadowRenderer::SetViewProjection(UINT index, D3DXMATRIX view, D3DXMATRIX projection)
{
	if (index >= viewProjectionDataContext.size())return;
	this->viewProjectionDataContext[index] = make_pair(view, projection);
}

void ShadowRenderer::CreatePointRenderer()
{

}

void ShadowRenderer::CreateDirectionalRenderer()
{
	Buffer::CreateDepthStencilSurface(&directionalTexture2D, &directionalSRV, &directionalDSV, DXGI_FORMAT_D16_UNORM, DXGI_FORMAT_R16_UNORM,
		WinSizeX, WinSizeY, 1);
}

void ShadowRenderer::UpdateViewProjection(UINT index)
{
	if (index >= viewProjectionDataContext.size())return;
	D3DXMATRIX view = this->viewProjectionDataContext[index].first;
	D3DXMATRIX proj = this->viewProjectionDataContext[index].second;
	D3DXMATRIX viewProj;
	D3DXMatrixMultiply(&viewProj, &view, &proj);

	this->viewProjectionBuffer->SetView(view);
	this->viewProjectionBuffer->SetProjection(proj);
	this->viewProjectionBuffer->SetVP(viewProj);

	this->viewProjectionBuffer->SetVSBuffer(0);
}
