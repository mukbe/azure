#include "stdafx.h"
#include "RenderTargetBuffer.h"

RenderTargetBuffer::RenderTargetBuffer(int width, int height, DXGI_FORMAT format)
	: renderTargetTexture(nullptr), rtv(nullptr), srv(nullptr),
	width(width), height(height), format(format), arraySize(1), bCubeMap(false)
{
}

RenderTargetBuffer::~RenderTargetBuffer()
{
	SAFE_RELEASE(renderTargetTexture);
	SAFE_RELEASE(rtv);
	SAFE_RELEASE(srv);
}

void RenderTargetBuffer::Create()
{
	if (bCubeMap)
	{
		CreateCubeBuffer();
	}
	else if (arraySize == 1)
	{
		CreateBuffer();
	}
	else
	{
		CreateArrayBuffer();
	}
}

void RenderTargetBuffer::CreateBuffer()
{
	//·»´õ Å¸°Ù¿ëÀ¸·Î »ç¿ëÇÒ ÅØ½ºÃÄ »ý¼º
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateTexture2D(&texDesc, 0, &renderTargetTexture);
	assert(SUCCEEDED(hr));

	//·»´õ Å¸°Ù ºä »ý¼º
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	hr = D3D::GetDevice()->CreateRenderTargetView(renderTargetTexture, &rtvDesc, &rtv);
	assert(SUCCEEDED(hr));

	//½¦ÀÌ´õ ¸®¼Ò½º ºä »ý¼º
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = format;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = D3D::GetDevice()->CreateShaderResourceView(renderTargetTexture, &srvDesc, &srv);
	assert(SUCCEEDED(hr));
}

void RenderTargetBuffer::CreateArrayBuffer()
{
	//·»´õ Å¸°Ù¿ëÀ¸·Î »ç¿ëÇÒ ÅØ½ºÃÄ »ý¼º
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = arraySize;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateTexture2D(&texDesc, 0, &renderTargetTexture);
	assert(SUCCEEDED(hr));

	//·»´õ Å¸°Ù ºä »ý¼º
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.FirstArraySlice = 0;
	rtvDesc.Texture2DArray.MipSlice = 0;
	rtvDesc.Texture2DArray.ArraySize = arraySize;

	hr = D3D::GetDevice()->CreateRenderTargetView(renderTargetTexture, &rtvDesc, &rtv);
	assert(SUCCEEDED(hr));

	//½¦ÀÌ´õ ¸®¼Ò½º ºä »ý¼º
	//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	//srvDesc.Format = format;
	//srvDesc.Texture2DArray.MipLevels = -1;
	//srvDesc.Texture2DArray.MostDetailedMip = 0;
	//srvDesc.Texture2DArray.FirstArraySlice = 0;
	//srvDesc.Texture2DArray.ArraySize = arraySize;
	hr = D3D::GetDevice()->CreateShaderResourceView(renderTargetTexture, nullptr, &srv);
	assert(SUCCEEDED(hr));
}

void RenderTargetBuffer::CreateCubeBuffer()
{
	//·»´õ Å¸°Ù¿ëÀ¸·Î »ç¿ëÇÒ ÅØ½ºÃÄ »ý¼º
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 6;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateTexture2D(&texDesc, 0, &renderTargetTexture);
	assert(SUCCEEDED(hr));

	//·»´õ Å¸°Ù ºä »ý¼º
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.FirstArraySlice = 0;
	rtvDesc.Texture2DArray.MipSlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 6;

	hr = D3D::GetDevice()->CreateRenderTargetView(renderTargetTexture, &rtvDesc, &rtv);
	assert(SUCCEEDED(hr));

	//½¦ÀÌ´õ ¸®¼Ò½º ºä »ý¼º
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Format = format;
	srvDesc.TextureCube.MipLevels = -1;
	srvDesc.TextureCube.MostDetailedMip = 0;
	hr = D3D::GetDevice()->CreateShaderResourceView(renderTargetTexture, &srvDesc, &srv);
	assert(SUCCEEDED(hr));
}
