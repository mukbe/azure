#pragma once
class InstanceShader
{
public:
	InstanceShader(wstring  fileName);
	~InstanceShader();
	void Render();

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



