#include "stdafx.h"
#include "DeferredRenderer.h"

#include "RenderTargetBuffer.h"
#include "./View/OrthoWindow.h"
#include "./Renders/WorldBuffer.h"

DeferredRenderer::DeferredRenderer()
{
	this->shader = new Shader(Shaders + L"002_Deferred.hlsl",Shader::ShaderType::Default,"BasicDeferred");

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	this->orthoWindow = new OrthoWindow(desc.Width, desc.Height);

	this->Create();
}


DeferredRenderer::~DeferredRenderer()
{
	for (int i = 0; i < BUFFER_COUNT; ++i)
	{
		SafeRelease(this->shaderResourceView[i]);
		SafeRelease(this->renderTargetView[i]);
		SafeRelease(this->renderTargetTexture[i]);
	}

	SafeRelease(depthBufferTexture);
	SafeRelease(depthStencilView);

	SafeDelete(shader);
	SafeDelete(orthoWindow);
}

void DeferredRenderer::BegindDrawToGBuffer()
{
	//렌더타겟을 잡아준다. 
	DeviceContext->OMSetRenderTargets(BUFFER_COUNT, renderTargetView, depthStencilView);
	DeviceContext->RSSetViewports(1, &viewport);

	this->ClearRenderTarget();
}

void DeferredRenderer::ClearRenderTarget()
{
	for (int i = 0; i<BUFFER_COUNT; i++)
	{
		DeviceContext->ClearRenderTargetView(renderTargetView[i], D3DXCOLOR(0.f, 0.f, 0.f,1.f));
	}

	DeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

}



void DeferredRenderer::Render()
{
	orthoWindow->Render();
	DeviceContext->PSSetShaderResources(0, BUFFER_COUNT, &shaderResourceView[0]);
	shader->Render();
	DeviceContext->DrawIndexed(6, 0, 0);
}

void DeferredRenderer::UIRender()
{
	ImGui::Begin("Deferred");
	{
		ImGui::ImageButton(shaderResourceView[0], ImVec2(200, 150)); ImGui::SameLine();
		ImGui::ImageButton(shaderResourceView[1], ImVec2(200, 150));
		ImGui::ImageButton(shaderResourceView[3], ImVec2(200, 150)); ImGui::SameLine();
		ImGui::ImageButton(depthSRV, ImVec2(200, 150));
	}
	ImGui::End();
}

bool DeferredRenderer::Create()
{
	HRESULT hr;

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);

	//RenderTargetView 를 생성하기 위한 텍스쳐 생성 
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = desc.Width;
	textureDesc.Height = desc.Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		hr = Device->CreateTexture2D(&textureDesc, NULL, &renderTargetTexture[i]);
		assert(SUCCEEDED(hr));
	}

	//RenderTargetViewDesc 설정
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i<BUFFER_COUNT; i++)
	{
		hr = Device->CreateRenderTargetView(renderTargetTexture[i], &renderTargetViewDesc, &renderTargetView[i]);
		assert(SUCCEEDED(hr));
	}

	//ResourceView 설정 
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i<BUFFER_COUNT; i++)
	{
		hr= Device->CreateShaderResourceView(renderTargetTexture[i], &shaderResourceViewDesc, &shaderResourceView[i]);
		assert(SUCCEEDED(hr));
	}

	//깊이 버퍼 텍스쳐 Desc
	D3D11_TEXTURE2D_DESC depthBufferDesc =
	{
		(UINT)desc.Width,			//UINT Width;
		(UINT)desc.Height,		//UINT Height;
		1,					//UINT MipLevels;
		1,					//UINT ArraySize;
		DXGI_FORMAT_UNKNOWN, //DXGI_FORMAT Format;
		1,						//DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,	//D3D11_USAGE Usage;
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags;
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
	};

	hr = Device->CreateTexture2D(&depthBufferDesc, NULL, &depthBufferTexture);
	assert(SUCCEEDED(hr));



	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	
	hr = Device->CreateDepthStencilView(depthBufferTexture, &depthStencilViewDesc, &depthStencilView);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC depthSRVDesc =
	{
		DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
		D3D11_SRV_DIMENSION_TEXTURE2D,
		0,
		0
	};
	depthSRVDesc.Texture2D.MipLevels = 1;
	hr = Device->CreateShaderResourceView(depthBufferTexture, &depthSRVDesc, &depthSRV);
	assert(SUCCEEDED(hr));

	//viewport설정
	viewport.Width = (float)desc.Width;
	viewport.Height = (float)desc.Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	return true;
}