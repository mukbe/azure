#include "stdafx.h"
#include "ComputeResource.h"



IComputeResource::IComputeResource()
	:srv(nullptr), uav(nullptr)
{
}


IComputeResource::~IComputeResource()
{
	SafeRelease(uav);
	SafeRelease(srv);
}

void IComputeResource::BindCSShaderResourceView(int slot)
{
	DeviceContext->CSSetShaderResources(slot, 1, &srv);
}
void IComputeResource::ReleaseCSshaderResorceView(int slot)
{
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };

	DeviceContext->CSSetShaderResources(slot, 1, nullSRV);
}

void IComputeResource::BindPSShaderResourceView(int slot)
{
	DeviceContext->PSSetShaderResources(slot, 1, &srv);
}

void IComputeResource::ReleaseShaderResourceView(int slot)
{
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };

	DeviceContext->PSSetShaderResources(slot, 1, nullSRV);
}

void IComputeResource::BindDSShaderResourceView(int slot)
{
	DeviceContext->DSSetShaderResources(slot, 1, &srv);
}

void IComputeResource::ReleaseDSshaderResorceView(int slot)
{
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };

	DeviceContext->DSSetShaderResources(slot, 1, nullSRV);
}

void IComputeResource::BindResource(int slot)
{
	DeviceContext->CSSetUnorderedAccessViews(slot, 1, &uav, nullptr);
}

void IComputeResource::ReleaseResource(int slot)
{
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	DeviceContext->CSSetUnorderedAccessViews(slot, 1, nullUAV, nullptr);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
CResource1D::CResource1D(UINT elementSize, UINT count, void * pInitData)
	:IComputeResource(), byteWidth(0), rwBuffer(nullptr), readBuffer(nullptr)
{
	byteWidth = elementSize * count;
	CreateBufferForGPU(elementSize, count, pInitData, rwBuffer);
	CreateBufferForGPU(elementSize, count, nullptr, readBuffer);
	CreateSRV();
	CreateUAV();
}

CResource1D::~CResource1D()
{
	SafeRelease(readBuffer);
	SafeRelease(rwBuffer);
}

void CResource1D::CreateBufferForGPU(UINT elementSize, UINT count, void * pInitData, ID3D11Buffer* buffer)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));


	HRESULT hr;
	if (rwBuffer == nullptr)
	{
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = elementSize;
		desc.ByteWidth = elementSize * count;
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA InitData = { 0 };
		InitData.pSysMem = pInitData;
		
		if(pInitData != nullptr)
			hr = Device->CreateBuffer(&desc, &InitData, &rwBuffer);
		else 
			hr = Device->CreateBuffer(&desc, nullptr, &rwBuffer);
	}
	else
	{
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.ByteWidth = elementSize * count;
		desc.StructureByteStride = elementSize;

		hr = Device->CreateBuffer(&desc, nullptr, &readBuffer);
	}
	assert(SUCCEEDED(hr));
}

void CResource1D::CreateSRV()
{
	D3D11_BUFFER_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));
	rwBuffer->GetDesc(&bufDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	if (bufDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = bufDesc.ByteWidth / 4;
	}
	else if (bufDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		// This is a Structured Buffer

		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.BufferEx.NumElements = bufDesc.ByteWidth / bufDesc.StructureByteStride;
	}
	HRESULT hr = Device->CreateShaderResourceView(rwBuffer, &desc, &srv);
	assert(SUCCEEDED(hr));

}

void CResource1D::CreateUAV()
{
	D3D11_BUFFER_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));
	rwBuffer->GetDesc(&bufDesc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if (bufDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = bufDesc.ByteWidth / 4;
	}
	else if(bufDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
		desc.Buffer.NumElements = bufDesc.ByteWidth / bufDesc.StructureByteStride;

	}
	//desc.Format = DXGI_FORMAT_UNKNOWN;      
	//desc.Buffer.NumElements = bufDesc.ByteWidth / bufDesc.StructureByteStride;



	HRESULT hr = Device->CreateUnorderedAccessView(rwBuffer, &desc, &uav);
	assert(SUCCEEDED(hr));

}
void CResource1D::GetDatas(void * datas)
{
	DeviceContext->CopyResource(readBuffer, rwBuffer);

	D3D11_MAPPED_SUBRESOURCE dat = { 0 };
	
	DeviceContext->Map(readBuffer, 0, D3D11_MAP_READ, 0, &dat);
	{
		memcpy(datas, dat.pData, byteWidth);
	}
	DeviceContext->Unmap(readBuffer, 0);

}

CResource2D::CResource2D(UINT width, UINT height, DXGI_FORMAT format, void* pInitData)
	:IComputeResource(), rwBuffer(nullptr),readBuffer(nullptr),format(format)
{
	CreateBufferForGPU(width, height, pInitData, rwBuffer);
	CreateBufferForGPU(width, height, nullptr, readBuffer);

	CreateSRV();
	CreateUAV();
}


CResource2D::~CResource2D()
{
}

void CResource2D::GetDatas(vector<vector<D3DXCOLOR>>& colors)
{
	ID3D11Resource* result;
	uav->GetResource(&result);
	DeviceContext->CopyResource(readBuffer, result);

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	readBuffer->GetDesc(&desc);

	D3D11_MAPPED_SUBRESOURCE dat;
	UINT w = 0;
	float* pixels = nullptr;
	UINT formatSize = sizeof(float);
	DeviceContext->Map(readBuffer, 0, D3D11_MAP_READ, 0, &dat);
	{
		w = dat.RowPitch / formatSize;
		pixels = new float[w * desc.Height * 4];
		memcpy(pixels, dat.pData, w * desc.Height * 4);
	}
	DeviceContext->Unmap(readBuffer, 0);

	vector<D3DXCOLOR> tempX;
	tempX.assign(desc.Width, D3DXCOLOR(0, 0, 0, 0));
	colors.assign(desc.Height, tempX);

	int index = 0;

	for (UINT y = 0; y < desc.Height; y++)
	{
		for (UINT x = 0; x < desc.Width; x++)
		{
			float a = pixels[index + 3];
			float b = pixels[index + 2];
			float g = pixels[index + 1];
			float r = pixels[index];

			colors[y][x] = D3DXCOLOR(r, g, b, a);
			index += 4;
		}
	}

}

void CResource2D::SavePNG(wstring path)
{
	HRESULT hr = D3DX11SaveTextureToFile
	(
		DeviceContext, readBuffer, D3DX11_IFF_PNG, path.c_str()
	);
	assert(SUCCEEDED(hr));
}

void CResource2D::UpdateSRV()
{
	ID3D11Resource* result;
	uav->GetResource(&result);
	DeviceContext->CopyResource(readBuffer, result);
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	readBuffer->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = desc.Format;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	HRESULT hr = Device->CreateShaderResourceView(rwBuffer, &srvDesc, &srv);

}

void CResource2D::CreateBufferForGPU(UINT width, UINT height, void * pInitData, ID3D11Texture2D * buffer)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Format = format;
	desc.Width = width;
	desc.Height = height;

	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;


	HRESULT hr;
	if (rwBuffer == nullptr)
	{
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

		hr = Device->CreateTexture2D(&desc, nullptr, &rwBuffer);
	}
	else
	{
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;

		hr = Device->CreateTexture2D(&desc, nullptr, &readBuffer);
	}
	assert(SUCCEEDED(hr));
}

void CResource2D::CreateSRV()
{

	D3D11_TEXTURE2D_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(D3D11_TEXTURE2D_DESC));
	rwBuffer->GetDesc(&bufDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = bufDesc.Format;
	srvDesc.Texture2D.MipLevels = bufDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	HRESULT hr = Device->CreateShaderResourceView(rwBuffer, &srvDesc, &srv);
	assert(SUCCEEDED(hr));

}

void CResource2D::CreateUAV()
{
	D3D11_TEXTURE2D_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(D3D11_TEXTURE2D_DESC));
	rwBuffer->GetDesc(&bufDesc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc;
	ZeroMemory(&viewDesc, sizeof(viewDesc));
	viewDesc.Format = bufDesc.Format;
	viewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipSlice = 0;
	
	HRESULT hr = Device->CreateUnorderedAccessView(rwBuffer, &viewDesc, &uav);
	assert(SUCCEEDED(hr));
}

CAppendResource1D::CAppendResource1D(UINT elementSize, UINT count, void * pInitData)
	:IComputeResource(), byteWidth(0), rwBuffer(nullptr), readBuffer(nullptr)
{
	byteWidth = elementSize * count;
	CreateBufferForGPU(elementSize, count, pInitData, rwBuffer);
	CreateBufferForGPU(elementSize, count, nullptr, readBuffer);
	CreateSRV();
	CreateUAV();
}

CAppendResource1D::~CAppendResource1D()
{
	SafeRelease(readBuffer);
	SafeRelease(rwBuffer);
}

void CAppendResource1D::GetDatas(void * datas)
{
	DeviceContext->CopyResource(readBuffer, rwBuffer);

	D3D11_MAPPED_SUBRESOURCE dat = { 0 };
	DeviceContext->Map(readBuffer, 0, D3D11_MAP_READ, 0, &dat);
	{
		memcpy(datas, dat.pData, byteWidth);
	}
	DeviceContext->Unmap(readBuffer, 0);
}

void CAppendResource1D::SetDatas(void * datas)
{
	D3D11_SUBRESOURCE_DATA InitData = { 0 };
	InitData.pSysMem = datas;

	DeviceContext->UpdateSubresource(rwBuffer, 0, nullptr, datas, byteWidth, 0);
}

void CAppendResource1D::CreateBufferForGPU(UINT elementSize, UINT count, void * pInitData, ID3D11Buffer * buffer)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));


	HRESULT hr;
	if (rwBuffer == nullptr)
	{
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = elementSize;
		desc.ByteWidth = elementSize * count;
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA InitData = { 0 };
		InitData.pSysMem = pInitData;

		if (pInitData != nullptr)
			hr = Device->CreateBuffer(&desc, &InitData, &rwBuffer);
		else
			hr = Device->CreateBuffer(&desc, nullptr, &rwBuffer);
	}
	else
	{
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.ByteWidth = elementSize * count;
		desc.StructureByteStride = elementSize;

		hr = Device->CreateBuffer(&desc, nullptr, &readBuffer);
	}
	assert(SUCCEEDED(hr));
}

void CAppendResource1D::CreateSRV()
{
	D3D11_BUFFER_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));
	rwBuffer->GetDesc(&bufDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	if (bufDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = bufDesc.ByteWidth / 4;
	}
	else if (bufDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		// This is a Structured Buffer

		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.BufferEx.NumElements = bufDesc.ByteWidth / bufDesc.StructureByteStride;
	}
	HRESULT hr = Device->CreateShaderResourceView(rwBuffer, &desc, &srv);
	assert(SUCCEEDED(hr));
}

void CAppendResource1D::CreateUAV()
{
	D3D11_BUFFER_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));
	rwBuffer->GetDesc(&bufDesc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if (bufDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		desc.Buffer.NumElements = bufDesc.ByteWidth / 4;
	}
	else if (bufDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
		desc.Buffer.NumElements = bufDesc.ByteWidth / bufDesc.StructureByteStride;

	}
	//desc.Format = DXGI_FORMAT_UNKNOWN;      
	//desc.Buffer.NumElements = bufDesc.ByteWidth / bufDesc.StructureByteStride;



	HRESULT hr = Device->CreateUnorderedAccessView(rwBuffer, &desc, &uav);
	assert(SUCCEEDED(hr));

}

CResource3D::CResource3D(UINT width, UINT height, UINT depth, DXGI_FORMAT format, void * pInitData)
	:IComputeResource(), rwBuffer(nullptr), readBuffer(nullptr), format(format)
{
	this->depth = depth;
	CreateBufferForGPU(width, height, depth, pInitData, rwBuffer);
	CreateBufferForGPU(width, height, depth, nullptr, readBuffer);
	CreateSRV();
	CreateUAV();

}

CResource3D::~CResource3D()
{
}

void CResource3D::CreateBufferForGPU(UINT width, UINT height, UINT depth, void * pInitData, ID3D11Texture3D * buffer)
{
	D3D11_TEXTURE3D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE3D_DESC));
	desc.Format = format;
	desc.Width = width;
	desc.Height = height;
	desc.Depth = depth;

	HRESULT hr;
	if (rwBuffer == nullptr)
	{
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

		hr = Device->CreateTexture3D(&desc, nullptr, &rwBuffer);
	}
	else
	{
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;

		hr = Device->CreateTexture3D(&desc, nullptr, &readBuffer);
	}
	assert(SUCCEEDED(hr));

}

void CResource3D::CreateSRV()
{
	D3D11_TEXTURE3D_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(D3D11_TEXTURE3D_DESC));
	rwBuffer->GetDesc(&bufDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	desc.Format = format;
	desc.Texture3D.MipLevels = -1;
	desc.Texture3D.MostDetailedMip = 0;

	HRESULT hr = Device->CreateShaderResourceView(rwBuffer, &desc, &srv);
	assert(SUCCEEDED(hr));

}

void CResource3D::CreateUAV()
{
	D3D11_TEXTURE3D_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(D3D11_TEXTURE3D_DESC));
	rwBuffer->GetDesc(&bufDesc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc;
	ZeroMemory(&viewDesc, sizeof(viewDesc));
	viewDesc.Format = bufDesc.Format;
	viewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	viewDesc.Texture3D.MipSlice = 0;
	viewDesc.Texture3D.FirstWSlice = 0;
	viewDesc.Texture3D.WSize = depth;

	HRESULT hr = Device->CreateUnorderedAccessView(rwBuffer, &viewDesc, &uav);

	assert(SUCCEEDED(hr));
}
