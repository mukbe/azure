#include "stdafx.h"
#include "ShadowRenderer.h"

#include "../Utilities/Buffer.h"
#include "WorldBuffer.h"

ShadowRenderer::ShadowRenderer()
	:directionalTexture2D(NULL), directionalDSV(NULL), directionalSRV(NULL), renderFunc(NULL)
{

	this->CreateDirectionalRenderer();


	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(WinSizeX );
	viewport.Height = static_cast<float>(WinSizeY);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof D3D11_SAMPLER_DESC);
	desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MaxAnisotropy = 1;
	HRESULT hr = Device->CreateSamplerState(&desc, &shadowSampler);
	assert(SUCCEEDED(hr));
	RenderRequest->AddRender("deui", bind(&ShadowRenderer::UIRender, this), RenderType::UIRender);

}


ShadowRenderer::~ShadowRenderer()
{
	SafeRelease(this->directionalSRV);
	SafeRelease(this->directionalDSV);
	SafeRelease(this->directionalTexture2D);

	SafeRelease(shadowSampler);
}

void ShadowRenderer::SetRTV()
{
	DeviceContext->RSSetViewports(1, &viewport);
	DeviceContext->ClearDepthStencilView(directionalDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	ID3D11RenderTargetView* nullRenderTarget = NULL;
	DeviceContext->OMSetRenderTargets(1, &nullRenderTarget,directionalDSV);

	if (this->renderFunc != NULL)
		this->renderFunc();
}

void ShadowRenderer::Render()
{
	DeviceContext->PSSetShaderResources(5, 1, &directionalSRV);
	DeviceContext->PSSetSamplers(2, 1, &shadowSampler);
}

void ShadowRenderer::UIRender()
{
	ImGui::Begin("ShadowMap");
	{
		ImGui::ImageButton(directionalSRV, ImVec2(200, 200));
	}
	ImGui::End();
}

void ShadowRenderer::CreateDirectionalRenderer()
{
	Buffer::CreateDepthStencilSurface(&directionalTexture2D, &directionalSRV, &directionalDSV, DXGI_FORMAT_D16_UNORM, DXGI_FORMAT_R16_UNORM,
		WinSizeX , WinSizeY , 1);
}