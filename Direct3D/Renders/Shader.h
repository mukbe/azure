#pragma once

class Shader
{
public:
	Shader(wstring shaderFile);
	~Shader();

	void Render();

	ID3D11VertexShader* GetVertexShader()const { return vertexShader; }
	ID3D11PixelShader* GetPixelShader()const { return pixelShader; }
	ID3D11InputLayout* GetInputLayout()const { return inputLayout; }

private:
	void CreateVertexShader();
	void CreatePixelShader();
	void CheckShaderError(HRESULT hr, ID3DBlob* error);
	void CreateInputLayout();

private:
	wstring shaderFile;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;

	ID3D11InputLayout* inputLayout;

	ID3DBlob* vertexBlob;
	ID3DBlob* pixelBlob;

	ID3D11ShaderReflection* reflection;
};