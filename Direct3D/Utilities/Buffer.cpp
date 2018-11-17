#include "stdafx.h"
#include "Buffer.h"

Buffer::Buffer()
{
}


Buffer::~Buffer()
{
}

void Buffer::CreateVertexBuffer(ID3D11Buffer ** buffer, void* vertex, UINT vertexSize)
{
	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = vertexSize;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = vertex;

	HRESULT hr = Device->CreateBuffer(&desc, &data, buffer);
	assert(SUCCEEDED(hr));
}

void Buffer::CreateIndexBuffer(ID3D11Buffer ** buffer, UINT* index, UINT indexCount)
{
	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = index;

	HRESULT hr = Device->CreateBuffer(&desc, &data, buffer);
	assert(SUCCEEDED(hr));
}

void Buffer::CreateDynamicVertexBuffer(ID3D11Buffer ** buffer, void * vertex, UINT vertexSize)
{

	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = vertexSize;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = vertex;

	HRESULT hr = Device->CreateBuffer(&desc, &data, buffer);
	assert(SUCCEEDED(hr));
}

void Buffer::CreateDynamicIndexBuffer(ID3D11Buffer ** buffer, void * vertex, UINT vertexSize)
{
	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = vertexSize;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = vertex;

	HRESULT hr = Device->CreateBuffer(&desc, &data, buffer);
	assert(SUCCEEDED(hr));
}

void Buffer::CreateShaderBuffer(ID3D11Buffer ** buffer, void * data, UINT bufferSize)
{
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = bufferSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	HRESULT hr = Device->CreateBuffer(&desc, NULL, buffer);
	assert(SUCCEEDED(hr));

}

void Buffer::UpdateBuffer(ID3D11Buffer**buffer, void * data, UINT dataSize)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	HRESULT hr = DeviceContext->Map
	(
		*buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	);

	memcpy(subResource.pData, data, dataSize);
	DeviceContext->Unmap(*buffer, 0);
}

void Buffer::CreateTexture(wstring fileName, ID3D11ShaderResourceView ** outTex)
{
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
		Device,
		fileName.c_str(),
		NULL,
		NULL,
		outTex,
		NULL
	);
	assert(SUCCEEDED(hr));
}

void Buffer::CreateTextureArray(ID3D11ShaderResourceView** pOut, vector<wstring>& fileNames, DXGI_FORMAT format, UINT filter, UINT mipFilter)
{
	UINT size = fileNames.size();
	HRESULT hr;
	std::vector<ID3D11Texture2D*> srcTex(size);
	for (UINT i = 0; i < size; ++i)
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;

		loadInfo.Width = D3DX11_FROM_FILE;
		loadInfo.Height = D3DX11_FROM_FILE;
		loadInfo.Depth = D3DX11_FROM_FILE;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = D3DX11_FROM_FILE;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = format;
		loadInfo.Filter = filter;
		loadInfo.MipFilter = mipFilter;
		loadInfo.pSrcInfo = 0;

		hr = D3DX11CreateTextureFromFile(Device, fileNames[i].c_str(),
			&loadInfo, 0, (ID3D11Resource**)&srcTex[i], 0);
		assert(SUCCEEDED(hr));
	}


	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = size;
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	hr = Device->CreateTexture2D(&texArrayDesc, 0, &texArray);
	assert(SUCCEEDED(hr));

	for (UINT texElement = 0; texElement < size; ++texElement)
	{
		for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			hr = DeviceContext->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D);
			assert(SUCCEEDED(hr));
			DeviceContext->UpdateSubresource(texArray,
				D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			DeviceContext->Unmap(srcTex[texElement], mipLevel);
		}
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	hr = Device->CreateShaderResourceView(texArray, &viewDesc, pOut);
	assert(SUCCEEDED(hr));

	SafeDelete(texArray);

	for (UINT i = 0; i < size; ++i)
		SafeDeleteArray(srcTex[i]);
}

void Buffer::CreateDepthStencilSurface(ID3D11Texture2D ** ppDepthStencilTexture, ID3D11ShaderResourceView ** ppDepthStencilSRV, ID3D11DepthStencilView ** ppDepthStencilView, 
	DXGI_FORMAT DSFormat, DXGI_FORMAT SRVFormat, UINT uWidth, UINT uHeight, UINT uSampleCount)
{
	HRESULT hr;
	DXGI_FORMAT TextureFormat;

	if (ppDepthStencilTexture)
	{
		switch (DSFormat)
		{
		case DXGI_FORMAT_D32_FLOAT:
			TextureFormat = DXGI_FORMAT_R32_TYPELESS;
			break;

		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			TextureFormat = DXGI_FORMAT_R24G8_TYPELESS;
			break;

		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_TYPELESS : 
			TextureFormat = DXGI_FORMAT_R16_TYPELESS;
			break;
		case  DXGI_FORMAT_R8G8B8A8_UINT :
			TextureFormat = DXGI_FORMAT_R8G8B8A8_UINT;
			break;
		default:
			TextureFormat = DXGI_FORMAT_UNKNOWN;
			break;
		}
		assert(TextureFormat != DXGI_FORMAT_UNKNOWN);

		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width = uWidth;
		descDepth.Height = uHeight;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = TextureFormat;
		descDepth.SampleDesc.Count = uSampleCount;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		if (NULL != ppDepthStencilSRV)
		{
			if ((descDepth.SampleDesc.Count == 1) || (pRenderer->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_1))
			{
				descDepth.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
			}
		}

		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = Device->CreateTexture2D(&descDepth, NULL, ppDepthStencilTexture);
		assert(SUCCEEDED(hr));

		if (NULL != ppDepthStencilView)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			descDSV.Format = DSFormat;
			if (descDepth.SampleDesc.Count > 1)
			{
				descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			}
			descDSV.Texture2D.MipSlice = 0;
			descDSV.Flags = 0;

			hr = Device->CreateDepthStencilView(*ppDepthStencilTexture, &descDSV, ppDepthStencilView);
			assert(SUCCEEDED(hr));
		}

		if (NULL != ppDepthStencilSRV)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SRDesc;
			SRDesc.Format = SRVFormat;
			SRDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRDesc.Texture2D.MostDetailedMip = 0;
			SRDesc.Texture2D.MipLevels = 1;

			hr = Device->CreateShaderResourceView(*ppDepthStencilTexture, &SRDesc, ppDepthStencilSRV);
			assert(SUCCEEDED(hr));
		}
	}
}
