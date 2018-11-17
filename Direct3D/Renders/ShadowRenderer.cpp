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

	RenderRequest->AddRender("shadowRenderer", bind(&ShadowRenderer::UIRender, this), RenderType::UIRender);

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

	States::SetRasterizer(States::RasterizerStates::SHADOW);

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

	////깊이 버퍼 텍스쳐 Desc
	//D3D11_TEXTURE2D_DESC depthBufferDesc =
	//{
	//	WinSizeX,						//UINT Width;
	//	WinSizeY,						//UINT Height;
	//	1,										//UINT MipLevels;
	//	1,										//UINT ArraySize;
	//	DXGI_FORMAT_R24G8_TYPELESS,				 //DXGI_FORMAT Format;
	//	1,										//DXGI_SAMPLE_DESC SampleDesc;
	//	0,
	//	D3D11_USAGE_DEFAULT,					//D3D11_USAGE Usage;
	//	D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE ,//UINT BindFlags;
	//	0,										//UINT CPUAccessFlags;
	//	0										//UINT MiscFlags;    
	//};
	//HRESULT hr = Device->CreateTexture2D(&depthBufferDesc, NULL, &directionalTexture2D);
	//assert(SUCCEEDED(hr));
	//
	//D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	//ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	//
	//depthStencilViewDesc.Flags = 0;
	//depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//depthStencilViewDesc.Texture2D.MipSlice = 0;
	//
	//hr = Device->CreateDepthStencilView(directionalTexture2D, &depthStencilViewDesc, &directionalDSV);
	//assert(SUCCEEDED(hr));
	//
	//D3D11_SHADER_RESOURCE_VIEW_DESC depthSRVDesc =
	//{
	//	DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
	//	D3D11_SRV_DIMENSION_TEXTURE2D,
	//	0,
	//	0
	//};
	//depthSRVDesc.Texture2D.MipLevels = 1;
	//hr = Device->CreateShaderResourceView(directionalTexture2D, &depthSRVDesc, &directionalSRV);
	//assert(SUCCEEDED(hr));

}