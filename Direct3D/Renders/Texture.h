//DirectXTex
#include <DirectXTex.h>
#pragma comment(lib, "directxtex.lib")

class Texture
{
public:
	Texture(DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	Texture(wstring file, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	Texture(wstring file, int width, int height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	~Texture();

	void SetTexture(wstring file);
	wstring GetFilePath() { return file; }

	int GetWidth() { return width; }
	int GetHeight() { return height; }
	void SetWidth(int val) { width = val; }
	void SetHeight(int val) { height = val; }
	DXGI_FORMAT GetFormat() { return format; }

	ID3D11ShaderResourceView* GetSRV() { return srv; }
	ID3D11ShaderResourceView* GetView() { return srv; }
	ID3D11Texture2D* GetTexture() { return texture; }

	void SetPixel(vector<D3DXCOLOR>& colors, int w, int h);
	void SetPixel32(vector<D3DXCOLOR>& colors, int w, int h);
	void GetPixel(vector<vector<D3DXCOLOR>>& colors);
	void GetPixel(vector<D3DXCOLOR>& colors);
	void GetPixel32(vector<vector<D3DXCOLOR>>& colors);

	void SavePNG(wstring saveFile);
	wstring GetFile()const { return file; }
	void SetShaderResource(UINT slot);
private:
	void Initialize();
	ID3D11Texture2D* GetReadBuffer();

private:
	wstring file;
	int width;
	int height;
	DXGI_FORMAT format;

	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;

	int lastSlot;
	UINT lastBindFlag;
};
