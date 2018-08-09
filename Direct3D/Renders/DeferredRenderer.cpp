#include "stdafx.h"
#include "DeferredRenderer.h"

#include "RenderTargetBuffer.h"
#include "OrthoWindow.h"
#include "./Renders/WorldBuffer.h"

DeferredRenderer::DeferredRenderer()
{
	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	

	this->shader = new Shader(Shaders + L"003_Texture.hlsl");
	this->orthoWindow = new OrthoWindow(desc.Width, desc.Height);
	this->viewProj = new ViewProjectionBuffer;
	D3DXMatrixOrthoLH(&matOrtho, (float)desc.Width, (float)desc.Height, 0.f, 1000.0f);

	this->Create();
}


DeferredRenderer::~DeferredRenderer()
{
	
}

void DeferredRenderer::SetRenderTarget()
{

	// Bind the render target view array and depth stencil buffer to the output render pipeline.
	DeviceContext->OMSetRenderTargets(BUFFER_COUNT, m_renderTargetViewArray, m_depthStencilView);

	// Set the viewport. 
	DeviceContext->RSSetViewports(1, &m_viewport);

	this->ClearRenderTarget();
}

void DeferredRenderer::ClearRenderTarget()
{
	
	// Clear the render target buffers.
	for (int i = 0; i<BUFFER_COUNT; i++)
	{
		DeviceContext->ClearRenderTargetView(m_renderTargetViewArray[i], D3DXCOLOR(0.0f,0.0f,0.0f,1.f));
	}

	// Clear the depth buffer.
	DeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}



void DeferredRenderer::Render()
{
	viewProj->SetProjection(matOrtho);
	viewProj->SetVSBuffer(0);

	orthoWindow->Render();

	DeviceContext->PSSetShaderResources(0, 4, &m_shaderResourceViewArray[0]);
	DeviceContext->PSSetShaderResources(4, 1, &depthSRV);

	shader->Render();

	DeviceContext->DrawIndexed(6,0,0);
}

void DeferredRenderer::PostRender()
{
	ImGui::Begin("Deferred");
	{
		ImGui::ImageButton(m_shaderResourceViewArray[0], ImVec2(200, 150)); ImGui::SameLine();
		ImGui::ImageButton(m_shaderResourceViewArray[1], ImVec2(200, 150));
		ImGui::ImageButton(m_shaderResourceViewArray[2], ImVec2(200, 150)); ImGui::SameLine();
		ImGui::ImageButton(m_shaderResourceViewArray[3], ImVec2(200, 150));
	}
	ImGui::End();
}

bool DeferredRenderer::Create()
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	int i;

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	// Store the width and height of the render texture.
	float m_textureWidth = 2048; desc.Width;
	float m_textureHeight = 2048; desc.Height;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
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

	// Create the render target textures.
	for (i = 0; i<BUFFER_COUNT; i++)
	{
		result = Device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTextureArray[i]);
		if (FAILED(result))
		{
			return false;
		}
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target views.
	for (i = 0; i<BUFFER_COUNT; i++)
	{
		result = Device->CreateRenderTargetView(m_renderTargetTextureArray[i], &renderTargetViewDesc, &m_renderTargetViewArray[i]);
		if (FAILED(result))
		{
			return false;
		}
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource views.
	for (i = 0; i<BUFFER_COUNT; i++)
	{
		result = Device->CreateShaderResourceView(m_renderTargetTextureArray[i], &shaderResourceViewDesc, &m_shaderResourceViewArray[i]);
		if (FAILED(result))
		{
			return false;
		}
	}

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = desc.Width;
	depthBufferDesc.Height = desc.Height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = Device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	assert(SUCCEEDED(result));

	// Initailze the depth stencil view description.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	
	// Create the depth stencil view.
	result = Device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	assert(SUCCEEDED(result));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = depthBufferDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	result = Device->CreateShaderResourceView(m_depthStencilBuffer, &srvDesc, &depthSRV);
	assert(SUCCEEDED(result));

	SafeRelease(m_depthStencilBuffer);

	// Setup the viewport for rendering.
	m_viewport.Width = (float)desc.Width;
	m_viewport.Height = (float)desc.Height;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	return true;
}