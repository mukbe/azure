#pragma once
//#include "ComputeShader\ComputeShaderResource.h"

class ComputeShader
{
public:
	ComputeShader(wstring shaderPath, string entryPoint = "CSMain");
	~ComputeShader();

	void BindShader();
	void Dispatch(int x, int y, int z);

private:
	void CheckShaderError(HRESULT hr, ID3D10Blob* error);

private:
	string entryPoint;
	wstring shaderFile;
	ID3DBlob * blobVS;
	
	ID3D11ComputeShader * computeShader;
};