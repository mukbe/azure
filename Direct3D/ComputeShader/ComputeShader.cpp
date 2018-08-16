#include "stdafx.h"
#include "ComputeShader.h"


void ComputeShader::BindShader()
{
	DeviceContext->CSSetShader(computeShader, NULL,	0);
}

void ComputeShader::Dispatch(int x, int y, int z)
{
	DeviceContext->Dispatch(x, y, z);
}


ComputeShader::ComputeShader(wstring shaderPath, string entryPoint)
	: shaderFile(shaderPath), entryPoint(entryPoint), blobVS(nullptr)
{
	ID3D10Blob* error;
	HRESULT hr = D3DX11CompileFromFile(
		shaderPath.c_str(),
		NULL,
		NULL,
		entryPoint.c_str(),
		"cs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL,
		&blobVS, 
		&error, NULL
	);
	CheckShaderError(hr, error);

	hr = Device->CreateComputeShader
	(
		blobVS->GetBufferPointer()
		, blobVS->GetBufferSize()
		, NULL
		, &computeShader
	);
	assert(SUCCEEDED(hr));

}

ComputeShader::~ComputeShader()
{
	SafeRelease(blobVS);
	SafeRelease(computeShader);
}

void ComputeShader::CheckShaderError(HRESULT hr, ID3D10Blob* error)
{
	if (FAILED(hr))
	{
		if (error != NULL)
		{
			string str = (const char *)error->GetBufferPointer();
			MessageBoxA(NULL, str.c_str(), "Shader Error", MB_OK);
		}
		assert(false);
	}
}
