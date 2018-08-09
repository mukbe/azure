#pragma once

class RenderTargetBuffer
{
public:
	RenderTargetBuffer(int width, int height, DXGI_FORMAT format);
	~RenderTargetBuffer();

	ID3D11RenderTargetView* GetRTV() { return rtv; }
	ID3D11ShaderResourceView* GetSRV() { return srv; }
	ID3D11DepthStencilView* GetDSV() { return dsv; }
	void SetArraySize(int size) { arraySize = size; }
	int GetArraySize() { return arraySize; }
	void SetCubeMap(bool set) { bCubeMap = set; arraySize = 6; }

	void Create();

private:
	void CreateBuffer();
	void CreateArrayBuffer();
	void CreateCubeBuffer();

private:
	int width;
	int height;
	DXGI_FORMAT format;

	int arraySize;
	bool bCubeMap;

	ID3D11Texture2D* renderTargetTexture;
	ID3D11RenderTargetView * rtv;
	ID3D11ShaderResourceView* srv;
	ID3D11DepthStencilView* dsv;
};