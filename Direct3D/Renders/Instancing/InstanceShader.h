#pragma once
class InstanceShader
{
public:
	InstanceShader(wstring  fileName,bool autoCreateLayout = true);
	~InstanceShader();
	void Render();

	void CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* descs, UINT layoutCount);
private:
	void CreateVertexShader();
	void CreatePixelShader();
	void CreateInputLayout();

private:
	wstring shaderFile;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;

	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
};



