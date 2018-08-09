#include "stdafx.h"
#include "Shader.h"

Shader::Shader(wstring shaderFile, ShaderType type , string funcName)
	:shaderFile(shaderFile), useType(type), reflection(nullptr), inputLayout(nullptr),
	vertexShader(nullptr), pixelShader(nullptr),hullShader(nullptr), domainshader(nullptr), geoShader(nullptr) , vertexBlob(nullptr)
{
	CreateShaderFromFile(funcName + "VS", &vertexBlob);
	CreateShaderFromFile(funcName + "PS");
	CreateInputLayout();

	bool b;
	
	if (b = ShaderType::useHS & type)
	{
		CreateShaderFromFile(funcName + "HS");
		CreateShaderFromFile(funcName + "DS");
	}

	if (b = ShaderType::useGS & type)
	{
		CreateShaderFromFile(funcName + "GS");
	}
}

Shader::~Shader()
{
	Release();
}

void Shader::Render()
{
	D3D::GetDC()->IASetInputLayout(inputLayout);

	D3D::GetDC()->VSSetShader(vertexShader, NULL, 0);
	D3D::GetDC()->PSSetShader(pixelShader, NULL, 0);

	bool b;
	if (b = ShaderType::useHS & useType)
	{
		D3D::GetDC()->HSSetShader(hullShader, NULL, 0);
		D3D::GetDC()->DSSetShader(domainshader, NULL, 0);
	}

	if (b = ShaderType::useGS & useType)
	{
		D3D::GetDC()->GSSetShader(geoShader, nullptr, 0);
	}

}

void Shader::Release()
{
	SafeRelease(reflection);
	SafeRelease(inputLayout);

	SafeRelease(vertexShader);
	SafeRelease(pixelShader);
	SafeRelease(vertexBlob);

	bool b;
	if (b = ShaderType::useHS & useType)
	{
		SafeRelease(hullShader);
		SafeRelease(domainshader);
	}

	if (b = ShaderType::useGS & useType)
	{
		SafeRelease(geoShader);
	}
}

void Shader::CheckShaderError(HRESULT hr, ID3DBlob * error)
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

void Shader::CreateShaderFromFile(string funcName, ID3DBlob** vsBlob)
{
	ID3DBlob* blob;
	HRESULT hr;

	if (strstr(funcName.c_str(), "VS"))
	{
		CompileFromFile(funcName, "vs_5_0", &blob);

		hr = D3D::GetDevice()->CreateVertexShader
		(
			blob->GetBufferPointer()
			, blob->GetBufferSize()
			, NULL
			, &vertexShader
		);
		assert(SUCCEEDED(hr));

	}
	else if (strstr(funcName.c_str(), "PS"))
	{
		CompileFromFile(funcName, "ps_5_0", &blob);

		hr = D3D::GetDevice()->CreatePixelShader
		(
			blob->GetBufferPointer()
			, blob->GetBufferSize()
			, NULL
			, &pixelShader
		);
		assert(SUCCEEDED(hr));
	}
	else if (strstr(funcName.c_str(), "HS"))
	{
		CompileFromFile(funcName, "hs_5_0", &blob);

		hr = D3D::GetDevice()->CreateHullShader
		(
			blob->GetBufferPointer()
			, blob->GetBufferSize()
			, NULL
			, &hullShader
		);
		assert(SUCCEEDED(hr));
	}
	else if (strstr(funcName.c_str(), "DS"))
	{
		CompileFromFile(funcName, "ds_5_0", &blob);

		hr = D3D::GetDevice()->CreateDomainShader
		(
			blob->GetBufferPointer()
			, blob->GetBufferSize()
			, NULL
			, &domainshader
		);
		assert(SUCCEEDED(hr));
	}
	else if (strstr(funcName.c_str(), "GS"))
	{
		CompileFromFile(funcName, "gs_5_0", &blob);

		hr = D3D::GetDevice()->CreateGeometryShader
		(
			blob->GetBufferPointer()
			, blob->GetBufferSize()
			, NULL
			, &geoShader
		);
		assert(SUCCEEDED(hr));
	}

	if (vsBlob)
		*vsBlob = blob;
	else
		SafeRelease(blob);


		

}

void Shader::CompileFromFile(string funcName, LPCSTR pProfile, ID3DBlob ** ppBlob)
{
	ID3D10Blob* error;
	HRESULT hr;

	hr = D3DX11CompileFromFile
	(
		shaderFile.c_str(), NULL, NULL, funcName.c_str(), pProfile
		, D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, ppBlob, &error, NULL
	);

	CheckShaderError(hr, error);
	assert(SUCCEEDED(hr));

	SafeRelease(error);

}

void Shader::CreateInputLayout()
{
	HRESULT hr;
	hr = D3DReflect
	(
		vertexBlob->GetBufferPointer()
		, vertexBlob->GetBufferSize()
		, IID_ID3D11ShaderReflection
		, (void**)&reflection
	);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i = 0; i< shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		reflection->GetInputParameterDesc(i, &paramDesc);

		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		string temp = paramDesc.SemanticName;
		if (temp == "POSITION")
			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

		inputLayoutDesc.push_back(elementDesc);
	}

	hr = D3D::GetDevice()->CreateInputLayout
	(
		&inputLayoutDesc[0]
		, inputLayoutDesc.size()
		, vertexBlob->GetBufferPointer()
		, vertexBlob->GetBufferSize()
		, &inputLayout
	);
	assert(SUCCEEDED(hr));

}
