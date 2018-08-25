#include "stdafx.h"
#include "InstanceShader.h"

InstanceShader::InstanceShader(wstring fileName)
	:shaderFile(fileName)
{
	this->CreateVertexShader();
	this->CreatePixelShader();
	this->CreateInputLayout();
}

InstanceShader::~InstanceShader()
{
	SafeRelease(layout);
	SafeRelease(pixelShader);
	SafeRelease(vertexShader);
}

void InstanceShader::Render()
{
	DeviceContext->IASetInputLayout(layout);
	DeviceContext->VSSetShader(vertexShader, NULL, 0);
	DeviceContext->PSSetShader(pixelShader, NULL, 0);
}

void InstanceShader::CreateVertexShader()
{
	HRESULT hr;

	ID3D10Blob* errorMessage;

	hr = D3DX10CompileFromFile
	(
		shaderFile.c_str(),
		NULL,
		NULL,
		"InstanceVS",
		"vs_5_0",		//셰이더의 버전 
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		NULL,
		&vertexShaderBuffer,
		&errorMessage,
		NULL
	);
	if (FAILED(hr))
	{
		if (errorMessage != NULL)
		{
			string str =
				(const char*)errorMessage->GetBufferPointer();
			MessageBoxA(NULL, str.c_str(),
				"Vertex Shader Error", MB_OK);
		}
		assert(false);
	}
	hr = Device->CreateVertexShader
	(
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		NULL,
		&vertexShader
	);
	assert(SUCCEEDED(hr));
}

void InstanceShader::CreatePixelShader()
{
	HRESULT hr;

	ID3D10Blob* errorMessage;


	hr = D3DX10CompileFromFile
	(
		this->shaderFile.c_str(),
		NULL,
		NULL,
		"InstancePS",
		"ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		NULL,
		&pixelShaderBuffer,
		&errorMessage,
		NULL
	);
	if (FAILED(hr))
	{
		if (errorMessage != NULL)
		{
			string str = (const char*)errorMessage->GetBufferPointer();
			MessageBoxA(NULL, str.c_str(), "Pixel Shader Error", MB_OK);
		}
		assert(false);
	}

	hr = Device->CreatePixelShader
	(
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(),
		NULL,
		&pixelShader
	);
}



void InstanceShader::CreateInputLayout()
{
	const D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[9] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,
		D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT ,0,12,
		D3D11_INPUT_PER_VERTEX_DATA,0 },
		{"BLENDINDICES",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,20,
		D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "BLENDWEIGHTS",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,36,
		D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT ,0,52,
		D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT ,0,64,
		D3D11_INPUT_PER_VERTEX_DATA,0 },

		{ "WORLD",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,0,
		D3D11_INPUT_PER_INSTANCE_DATA,1 },
		{ "WORLD",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,16,
		D3D11_INPUT_PER_INSTANCE_DATA,1 },
		{ "WORLD",2,DXGI_FORMAT_R32G32B32A32_FLOAT,1,32,
		D3D11_INPUT_PER_INSTANCE_DATA,1 },
	};


	HRESULT hr = Device->CreateInputLayout
	(
		inputLayoutDesc,
		9,
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		&layout
	);
	assert(SUCCEEDED(hr));
}
