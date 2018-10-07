#include "stdafx.h"
#include "DeferredRenderer.h"

#include "RenderTargetBuffer.h"
#include "./View/OrthoWindow.h"
#include "./Renders/WorldBuffer.h"

#include "./Renders/DepthVis.h"

DeferredRenderer::DeferredRenderer()
{
	this->shader = Shaders->CreateShader("Deferred",L"002_Deferred.hlsl", Shader::ShaderType::Default, "BasicDeferred");

	this->orthoWindow = new OrthoWindow(D3DXVECTOR2(-WinSizeX/2, WinSizeY/2),D3DXVECTOR2(WinSizeX,WinSizeY));
	this->deferredRenderingBuffer = new RenderTargetBuffer(WinSizeX, WinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT);
	this->deferredRenderingBuffer->SetArraySize(1);
	this->deferredRenderingBuffer->Create();

	this->Create();
	depthVis = new DepthVis;
	unPacker = new UnPacker;

	RenderRequest->AddRender("deui", bind(&DeferredRenderer::UIRender, this), RenderType::UIRender);
}


DeferredRenderer::~DeferredRenderer()
{
	SafeDelete(deferredRenderingBuffer);

	for (int i = 0; i < BUFFER_COUNT; ++i)
	{
		SafeRelease(this->mrtSRV[i]);
		SafeRelease(this->mrtView[i]);
		SafeRelease(this->mrtTexture[i]);
	}

	
	SafeRelease(mrtDepthBufferSRV);
	SafeRelease(mrtDepthBufferTexture);
	SafeRelease(depthStencilView);
	SafeDelete(shader);
	SafeDelete(orthoWindow);
	SafeDelete(unPacker);
}

void DeferredRenderer::SetRTV()
{
	//렌더타겟을 잡아준다. 
	DeviceContext->OMSetRenderTargets(BUFFER_COUNT, mrtView, depthStencilView);
	DeviceContext->RSSetViewports(1, &viewport);
	//DeviceContext->OMSetDepthStencilState(depthStencilState, 2);
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		DeviceContext->ClearRenderTargetView(mrtView[i], D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
	}
	
	DeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}



void DeferredRenderer::Render()
{
	deferredRenderingBuffer->BindRenderTarget();

	pRenderer->ChangeZBuffer(false);
	orthoWindow->Render();
	DeviceContext->PSSetShaderResources(0, BUFFER_COUNT, &mrtSRV[0]);
	DeviceContext->PSSetShaderResources(4, 1, &mrtDepthBufferSRV);
	unPacker->SetPSBuffer(2);

	shader->Render();
	orthoWindow->DrawIndexed();

	depthVis->CalcuDepth(mrtDepthBufferSRV);
	pRenderer->ChangeZBuffer(true);
}

void DeferredRenderer::UIRender()
{
	ImGui::Begin("Deferred");
	{
		static float fSize = 100.0f;
		ImGui::SliderFloat("ImageSize",&fSize,50.0f,500.0f);
		ImVec2 size = ImVec2(fSize, fSize);

		ImGui::ImageButton(mrtSRV[0], size); ImGui::SameLine();
		ImGui::ImageButton(mrtSRV[1], size);
		ImGui::ImageButton(mrtSRV[2], size); ImGui::SameLine();
		ImGui::ImageButton(depthVis->GetSRV(), size);
	}
	ImGui::End();
}

void DeferredRenderer::SetUnPackInfo(D3DXMATRIX view, D3DXMATRIX projection)
{
	unPacker->SetInvView(view);
	unPacker->SetPerspectiveValues(projection);
}

ID3D11ShaderResourceView * DeferredRenderer::GetRenderTargetSRV()
{
	if (deferredRenderingBuffer)
		return deferredRenderingBuffer->GetSRV();
	return nullptr;
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
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;		//HDR Rendering
	//textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		hr = Device->CreateTexture2D(&textureDesc, NULL, &mrtTexture[i]);
		assert(SUCCEEDED(hr));
	}

	//RenderTargetViewDesc 설정
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i<BUFFER_COUNT; i++)
	{
		hr = Device->CreateRenderTargetView(mrtTexture[i], &renderTargetViewDesc, &mrtView[i]);
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
		hr= Device->CreateShaderResourceView(mrtTexture[i], &shaderResourceViewDesc, &mrtSRV[i]);
		assert(SUCCEEDED(hr));
	}


	//깊이 버퍼 텍스쳐 Desc
	D3D11_TEXTURE2D_DESC depthBufferDesc =
	{
		(UINT)desc.Width,						//UINT Width;
		(UINT)desc.Height,						//UINT Height;
		1,										//UINT MipLevels;
		1,										//UINT ArraySize;
		DXGI_FORMAT_R24G8_TYPELESS,				 //DXGI_FORMAT Format;
		1,										//DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,					//D3D11_USAGE Usage;
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE ,//UINT BindFlags;
		0,										//UINT CPUAccessFlags;
		0										//UINT MiscFlags;    
	};
	hr = Device->CreateTexture2D(&depthBufferDesc, NULL, &mrtDepthBufferTexture);
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	
	hr = Device->CreateDepthStencilView(mrtDepthBufferTexture, &depthStencilViewDesc, &depthStencilView);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC depthSRVDesc =
	{
		DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
		D3D11_SRV_DIMENSION_TEXTURE2D,
		0,
		0
	};
	depthSRVDesc.Texture2D.MipLevels = 1;
	hr = Device->CreateShaderResourceView(mrtDepthBufferTexture, &depthSRVDesc, &mrtDepthBufferSRV);
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_DESC descDepth;
	descDepth.DepthEnable = TRUE;
	descDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepth.DepthFunc = D3D11_COMPARISON_LESS;
	descDepth.StencilEnable = TRUE;
	descDepth.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	descDepth.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
	descDepth.FrontFace = stencilMarkOp;
	descDepth.BackFace = stencilMarkOp;
	hr = Device->CreateDepthStencilState(&descDepth, &this->depthStencilState);
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