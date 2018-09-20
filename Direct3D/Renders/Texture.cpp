#include "stdafx.h"
#include "Texture.h"

#include "./Utilities/Path.h"

using namespace DirectX;

void Texture::Initialize()
{
	width = 0; height = 0;
	texture = nullptr; srv = nullptr;
	lastSlot = -1; lastBindFlag = 0;
	isSlot = ShaderSlot::None;
}

void Texture::SaveToFile(wstring fileFullPath, ID3D11ShaderResourceView * srv)
{
	HRESULT hr;
	ScratchImage out;
	GUID guid;

	wstring ext = Path::GetExtension(fileFullPath);

	//get GUID code
	if (ext == L"png")
		guid = GetWICCodec(WICCodecs::WIC_CODEC_PNG);
	else if (ext == L"bmp")
		guid = GetWICCodec(WICCodecs::WIC_CODEC_BMP);
	else if (ext == L"jpeg")
		guid = GetWICCodec(WICCodecs::WIC_CODEC_JPEG);
	else if (ext == L"tiff")
		guid = GetWICCodec(WICCodecs::WIC_CODEC_TIFF);
	else if (ext == L"gif")
		guid = GetWICCodec(WICCodecs::WIC_CODEC_GIF);
	else if (ext == L"wmp")
		guid = GetWICCodec(WICCodecs::WIC_CODEC_WMP);
	else if (ext == L"ico")
		guid = GetWICCodec(WICCodecs::WIC_CODEC_ICO);

	//get srv resorce
	ID3D11Resource* res;
	srv->GetResource(&res);

	//capture resource to scratchImage
	hr = CaptureTexture(Device, DeviceContext, res, out);
	assert(SUCCEEDED(hr));

	//get ImageCount, images
	const Image* image = out.GetImages();
	size_t num = out.GetImageCount();

	hr = SaveToWICFile(*image, DirectX::WIC_FLAGS_NONE, guid, (fileFullPath).c_str());
	assert(SUCCEEDED(hr));

}

Texture::Texture(DXGI_FORMAT format)
{
	Initialize();
	this->format = format;
}

Texture::Texture(wstring file, DXGI_FORMAT format)
{
	Initialize();
	this->file = file;
	this->format = format;

	this->SetTexture(file);
}

Texture::Texture(wstring file, int width, int height, DXGI_FORMAT format)
{
	Initialize();
	this->file = file;
	this->width = width;
	this->height = height;
	this->format = format;

	this->SetTexture(file);
}

Texture::~Texture()
{
	SafeRelease(texture);
	SafeRelease(srv);
}

void Texture::SetTexture(wstring file)
{
	SafeRelease(texture);
	SafeRelease(srv);

	HRESULT hr;

	//설정된 width, height가 없을 경우, 이미지의 크기로 셋팅한다.
	auto SetWidthHeight = [](int& width, int& height, UINT& texWidth, UINT& texHeight)
	{
		if (width == 0 || height == 0)
		{
			width = texWidth;
			height = texHeight;
		}
		else
		{
			texWidth = width;
			texHeight = height;
		}
	};

	TexMetadata metaData;
	ScratchImage image;
	wstring ext = Path::GetExtension(file);
	if (ext == L"tga")
	{
		hr = GetMetadataFromTGAFile(file.c_str(), metaData);
		assert(SUCCEEDED(hr));

		SetWidthHeight(width, height, metaData.width, metaData.height);

		hr = LoadFromTGAFile(file.c_str(), &metaData, image);
		assert(SUCCEEDED(hr));

		hr = DirectX::CreateShaderResourceView(Device, image.GetImages(), image.GetImageCount(), metaData, &srv);
		assert(SUCCEEDED(hr));
	}
	else if (ext == L"dds")
	{
		hr = GetMetadataFromDDSFile(file.c_str(), DDS_FLAGS_NONE, metaData);
		assert(SUCCEEDED(hr));

		SetWidthHeight(width, height, metaData.width, metaData.height);

		hr = LoadFromDDSFile(file.c_str(), DDS_FLAGS_NONE, &metaData, image);
		assert(SUCCEEDED(hr));

		hr = DirectX::CreateShaderResourceView(Device, image.GetImages(), image.GetImageCount(), metaData, &srv);
		assert(SUCCEEDED(hr));
	}
	else if (ext == L"hdr")
	{
		assert(false);
	}
	else if (ext == L"png" || ext == L"jpg")
	{
		if (width == 0 || height == 0)
		{
			D3DX11_IMAGE_INFO info;
			hr = D3DX11GetImageInfoFromFile
			(
				file.c_str(), NULL, &info, NULL
			);
			assert(SUCCEEDED(hr));

			width = info.Width;
			height = info.Height;
		}

		D3DX11_IMAGE_LOAD_INFO imageLoadInfo;
		imageLoadInfo.Width = width;
		imageLoadInfo.Height = height;
		imageLoadInfo.Depth = 0;
		imageLoadInfo.FirstMipLevel = 0;
		imageLoadInfo.MipLevels = 0;
		imageLoadInfo.Usage = D3D11_USAGE_DEFAULT;
		imageLoadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		imageLoadInfo.CpuAccessFlags = 0;
		imageLoadInfo.MiscFlags = 0;
		imageLoadInfo.Format = format;
		imageLoadInfo.Filter = D3DX11_FILTER_LINEAR;
		imageLoadInfo.MipFilter = D3DX11_FILTER_LINEAR;
		imageLoadInfo.pSrcInfo = NULL;

		D3DX11CreateTextureFromFile(Device, file.c_str(), &imageLoadInfo, nullptr,
			(ID3D11Resource**)&texture, nullptr);

		//SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Format = format;
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		hr = Device->CreateShaderResourceView(texture, &srvDesc, &srv);
		assert(SUCCEEDED(hr));
	}
	else
	{
		hr = GetMetadataFromWICFile(file.c_str(), WIC_FLAGS_NONE, metaData);
		assert(SUCCEEDED(hr));

		SetWidthHeight(width, height, metaData.width, metaData.height);

		hr = LoadFromWICFile(file.c_str(), WIC_FLAGS_NONE, &metaData, image);
		assert(SUCCEEDED(hr));

		hr = DirectX::CreateShaderResourceView(Device, image.GetImages(), image.GetImageCount(), metaData, &srv);
		assert(SUCCEEDED(hr));
	}
}

void Texture::SavePNG(wstring saveFile)
{
	HRESULT hr = D3DX11SaveTextureToFile
	(
		DeviceContext, texture, D3DX11_IFF_PNG, saveFile.c_str()
	);
	assert(SUCCEEDED(hr));
}

void Texture::SetShaderResource(UINT slot)
{
	DeviceContext->PSSetShaderResources(slot, 1, &srv);
	isSlot = isSlot | ShaderSlot::PS;
}

void Texture::SetCSResource(UINT slot)
{
	DeviceContext->CSSetShaderResources(slot, 1, &srv);
	isSlot = isSlot | ShaderSlot::CS;
}

void Texture::ReleaseResource()
{
}


void Texture::SetPixel(vector<D3DXCOLOR>& pixels, int w, int h)
{
	width = w;
	height = h;

	HRESULT hr;
	//쓰기용 텍스쳐를 생성한다.
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Width = w;
	desc.Height = h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ID3D11Texture2D* pTexture = NULL;
	hr = Device->CreateTexture2D(&desc, NULL, &pTexture);
	assert(SUCCEEDED(hr));

	//색상정보 입력하기.
	D3D11_MAPPED_SUBRESOURCE mapResource;
	DeviceContext->Map(pTexture, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_DISCARD, NULL, &mapResource);
	{
		BYTE* data = (BYTE*)mapResource.pData;
		int index = 0;
		int vIndex = 0;
		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				data[index + 0] = (BYTE)(pixels[vIndex].r * 255.0f);
				data[index + 1] = (BYTE)(pixels[vIndex].g * 255.0f);
				data[index + 2] = (BYTE)(pixels[vIndex].b * 255.0f);
				data[index + 3] = (BYTE)(pixels[vIndex].a * 255.0f);

				++vIndex;
				index += 4;
			}
		}
	}
	DeviceContext->Unmap(pTexture, D3D11CalcSubresource(0, 0, 1));

	//데이터가 입력된 텍스쳐로 뷰를 생성한다.
	SafeDelete(srv);

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	viewDesc.Format = desc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1;

	hr = Device->CreateShaderResourceView(pTexture, &viewDesc, &srv);
	assert(SUCCEEDED(hr));

	//GPU 전용 텍스처 생성.
	SafeRelease(texture);
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Format = format;
	texDesc.Width = w;
	texDesc.Height = h;
	texDesc.CPUAccessFlags = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;

	hr = Device->CreateTexture2D(&texDesc, nullptr, &texture);
	assert(SUCCEEDED(hr));

	ID3D11Resource* resource;
	srv->GetResource(&resource);
	DeviceContext->CopyResource(texture, resource);

	hr = D3DX11LoadTextureFromTexture(DeviceContext, pTexture, NULL, texture);

	SafeRelease(pTexture);
	SafeRelease(resource);
}

void Texture::SetPixel32(vector<D3DXCOLOR>& pixels, int w, int h)
{
	width = w;
	height = h;

	HRESULT hr;
	//쓰기용 텍스쳐를 생성한다.
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Width = w;
	desc.Height = h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ID3D11Texture2D* pTexture = NULL;
	hr = Device->CreateTexture2D(&desc, NULL, &pTexture);
	assert(SUCCEEDED(hr));

	//색상정보 입력하기.
	D3D11_MAPPED_SUBRESOURCE mapResource;
	DeviceContext->Map(pTexture, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_DISCARD, NULL, &mapResource);
	{
		float* pTexels = (float*)mapResource.pData;
		for (UINT row = 0; row < desc.Height; ++row)
		{
			//rowPitch == 이미지 1줄의 길이
			UINT rowStart = row * mapResource.RowPitch;

			for (UINT col = 0; col < desc.Width; ++col)
			{
				UINT index = (row*desc.Width) + col;

				UINT colStart = col * 4; // 4byte

				pTexels[rowStart + colStart + 0] = pixels[index].r;
				pTexels[rowStart + colStart + 1] = pixels[index].g;
				pTexels[rowStart + colStart + 2] = pixels[index].b;
				pTexels[rowStart + colStart + 3] = pixels[index].a;
			}
		}
	}
	DeviceContext->Unmap(pTexture, D3D11CalcSubresource(0, 0, 1));

	//데이터가 입력된 텍스쳐로 뷰를 생성한다.
	SafeRelease(srv);

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	viewDesc.Format = desc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 0;

	hr = Device->CreateShaderResourceView(pTexture, &viewDesc, &srv);
	assert(SUCCEEDED(hr));

	//GPU 전용 텍스처 생성.
	SafeRelease(texture);
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Format = format;
	texDesc.Width = w;
	texDesc.Height = h;
	texDesc.CPUAccessFlags = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;

	hr = Device->CreateTexture2D(&texDesc, nullptr, &texture);
	assert(SUCCEEDED(hr));

	ID3D11Resource* resource;
	srv->GetResource(&resource);
	DeviceContext->CopyResource(texture, resource);

	hr = D3DX11LoadTextureFromTexture(DeviceContext, pTexture, NULL, texture);

	SafeRelease(pTexture);
	SafeRelease(resource);
}

void Texture::GetPixel(vector<vector<D3DXCOLOR>>& colors)
{
	//읽기 전용 버퍼를 만든다.
	ID3D11Texture2D* readBuffer = GetReadBuffer();
	if (!readBuffer) return;

	ID3D11Resource* result;
	srv->GetResource(&result);
	DeviceContext->CopyResource(readBuffer, result);

	///////////////////////////////////////////////////////////////
	// 픽셀 복사
	///////////////////////////////////////////////////////////////
	D3D11_MAPPED_SUBRESOURCE dat;
	UINT w = 0;
	UINT* pixels = nullptr;
	UINT formatSize = sizeof(UINT);
	DeviceContext->Map(readBuffer, 0, D3D11_MAP_READ, 0, &dat);
	{
		w = dat.RowPitch / formatSize;
		pixels = new UINT[w * height];
		memcpy(pixels, dat.pData, formatSize * width * height);
	}
	DeviceContext->Unmap(readBuffer, 0);

	vector<D3DXCOLOR> tempX;
	tempX.assign(width, D3DXCOLOR(0, 0, 0, 0));
	colors.assign(height, tempX);

	int index = 0;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			CONST FLOAT f = 1.0f / 255.0f;
			float a = f * (float)((0xFF000000 & pixels[index]) >> 24);
			float b = f * (float)((0x00FF0000 & pixels[index]) >> 16);
			float g = f * (float)((0x0000FF00 & pixels[index]) >> 8);
			float r = f * (float)((0x000000FF & pixels[index]) >> 0);

			colors[y][x] = D3DXCOLOR(r, g, b, a);
			++index;
		}
	}

	SafeRelease(result);
	SafeRelease(readBuffer);
}

void Texture::GetPixel(vector<D3DXCOLOR>& colors)
{
	//읽기 전용 버퍼를 만든다.
	ID3D11Texture2D* readBuffer = GetReadBuffer();
	if (!readBuffer) return;

	ID3D11Resource* resource;
	srv->GetResource(&resource);
	DeviceContext->CopyResource(readBuffer, resource);

	///////////////////////////////////////////////////////////////
	// 픽셀 복사
	///////////////////////////////////////////////////////////////
	D3D11_MAPPED_SUBRESOURCE map;
	UINT w = width;
	UINT* pixels = nullptr;

	DeviceContext->Map(readBuffer, 0, D3D11_MAP_READ, NULL, &map);
	{
		w = map.RowPitch / sizeof(UINT);
		pixels = new UINT[w * height];
		memcpy(pixels, map.pData, sizeof(UINT) * w * height);
	}
	DeviceContext->Unmap(readBuffer, 0);

	colors.assign(width*height, D3DXCOLOR(0, 0, 0, 0));
	int index = 0;
	int vIndex = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			CONST FLOAT f = 1.0f / 255.0f;
			float a = f * (float)((0xFF000000 & pixels[index]) >> 24);
			float b = f * (float)((0x00FF0000 & pixels[index]) >> 16);
			float g = f * (float)((0x0000FF00 & pixels[index]) >> 8);
			float r = f * (float)((0x000000FF & pixels[index]) >> 0);

			colors[vIndex++] = D3DXCOLOR(r, g, b, a);
			++index;
		}
	}

	SafeRelease(resource);
	SafeRelease(readBuffer);
}

void Texture::GetPixel32(vector<vector<D3DXCOLOR>>& colors)
{
	//읽기 전용 버퍼를 만든다.
	ID3D11Texture2D* readBuffer = GetReadBuffer();
	if (!readBuffer) return;

	ID3D11Resource* result;
	srv->GetResource(&result);
	DeviceContext->CopyResource(readBuffer, result);

	///////////////////////////////////////////////////////////////
	// 픽셀 복사
	///////////////////////////////////////////////////////////////

	D3D11_MAPPED_SUBRESOURCE dat;
	UINT w = 0;
	float* pixels = nullptr;
	UINT formatSize = sizeof(float);
	DeviceContext->Map(readBuffer, 0, D3D11_MAP_READ, 0, &dat);
	{
		w = dat.RowPitch / formatSize;
		pixels = new float[w * height * 4];
		memcpy(pixels, dat.pData, w * height * 4);
	}
	DeviceContext->Unmap(readBuffer, 0);

	vector<D3DXCOLOR> tempX;
	tempX.assign(width, D3DXCOLOR(0, 0, 0, 0));
	colors.assign(height, tempX);

	int index = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float a = pixels[index + 3];
			float b = pixels[index + 2];
			float g = pixels[index + 1];
			float r = pixels[index];

			colors[y][x] = D3DXCOLOR(r, g, b, a);
			index += 4;
		}
	}

	SafeRelease(result);
	SafeRelease(readBuffer);
}


ID3D11Texture2D * Texture::GetReadBuffer()
{
	if (width <= 0 || height <= 0) return nullptr;
	ID3D11Texture2D* readBuffer;

	//읽기전용 버퍼
	D3D11_TEXTURE2D_DESC readDesc;
	ZeroMemory(&readDesc, sizeof(readDesc));
	readDesc.Format = format;
	readDesc.Width = width;
	readDesc.Height = height;
	readDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	readDesc.Usage = D3D11_USAGE_STAGING;
	readDesc.SampleDesc.Count = 1;
	readDesc.SampleDesc.Quality = 0;
	readDesc.MipLevels = 0;
	readDesc.ArraySize = 1;

	HRESULT hr = Device->CreateTexture2D(&readDesc, nullptr, &readBuffer);
	assert(SUCCEEDED(hr));

	return readBuffer;
}