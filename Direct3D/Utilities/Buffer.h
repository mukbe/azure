#pragma once
class Buffer
{
public:
	Buffer();
	~Buffer();

	static void CreateVertexBuffer(ID3D11Buffer** buffer, void* vertex, UINT vertexSize);
	static void CreateIndexBuffer(ID3D11Buffer**buffer, UINT* index, UINT indexCount);
	static void CreateDynamicVertexBuffer(ID3D11Buffer** buffer, void* vertex, UINT vertexSize);
	static void CreateDynamicIndexBuffer(ID3D11Buffer** buffer, void* vertex, UINT vertexSize);

	static void CreateShaderBuffer(ID3D11Buffer**buffer, void* data, UINT bufferSize);
	static void UpdateBuffer(ID3D11Buffer**buffer, void* data, UINT dataSize);
	static void CreateTexture(wstring fileName, ID3D11ShaderResourceView** outTex);
	static void CreateTextureArray(ID3D11ShaderResourceView** pOut, vector<wstring>& fileNames,
		DXGI_FORMAT format = DXGI_FORMAT_FROM_FILE,
		UINT filter = D3DX11_FILTER_NONE,
		UINT mipFilter = D3DX11_FILTER_LINEAR);

	static void CreateDepthStencilSurface(ID3D11Texture2D** ppDepthStencilTexture, ID3D11ShaderResourceView** ppDepthStencilSRV,
		ID3D11DepthStencilView** ppDepthStencilView, DXGI_FORMAT DSFormat, DXGI_FORMAT SRVFormat,
		UINT uWidth, UINT uHeight, UINT uSampleCount);
};