#pragma once

class Shader
{
public:
	Shader(wstring shaderFile);
	~Shader();

	wstring GetFileName() { return shaderFile; }

	void Bind();
	void Release();
private:
	void CheckShaderError(HRESULT hr, ID3DBlob* error);

	void CompileShader();
	void CreateInputLayout();

private:
	wstring shaderFile;

	ID3D11ShaderReflection* reflection;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11HullShader* hullShader;
	ID3D11DomainShader* domainshader;
	ID3D11GeometryShader* geoShader;

	ID3D11InputLayout* inputLayout;

	ID3DBlob* vertexBlob;
	ID3DBlob* pixelBlob;
	ID3DBlob* hullBlob;
	ID3DBlob* domainBlob;
	ID3DBlob* geoBlob;
};

