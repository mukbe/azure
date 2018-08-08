//#include "stdafx.h"
//#include "States.h"
//
//D3D11_RASTERIZER_DESC States::rasterizerDesc;
//D3D11_DEPTH_STENCIL_DESC States::depthStencilDesc;
//D3D11_SAMPLER_DESC States::samplerDesc;
//D3D11_BLEND_DESC States::blendDesc;
//
//void States::Create()
//{
//	CreateRasterizerDesc();
//	CreateDepthStencilDesc();
//	CreateSamplerDesc();
//	CreateBlendDesc();
//}
//
//void States::Delete()
//{
//}
//
//void States::GetRasterizerDesc(D3D11_RASTERIZER_DESC* desc)
//{
//	memcpy(desc, &rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
//}
//
//void States::CreateRasterizer(D3D11_RASTERIZER_DESC* desc, ID3D11RasterizerState** state)
//{
//	HRESULT hr;
//
//	hr = D3D::GetDevice()->CreateRasterizerState(desc, state);
//	assert(SUCCEEDED(hr));
//}
//
//void States::GetDepthStencilDesc(D3D11_DEPTH_STENCIL_DESC* desc)
//{
//	memcpy(desc, &depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
//}
//
//void States::CreateDepthStencil(D3D11_DEPTH_STENCIL_DESC * desc, ID3D11DepthStencilState ** state)
//{
//	HRESULT hr;
//
//	hr = D3D::GetDevice()->CreateDepthStencilState(desc, state);
//	assert(SUCCEEDED(hr));
//}
//
//void States::GetSamplerDesc(D3D11_SAMPLER_DESC * desc)
//{
//	memcpy(desc, &samplerDesc, sizeof(D3D11_SAMPLER_DESC));
//}
//
//void States::CreateSampler(D3D11_SAMPLER_DESC * desc, ID3D11SamplerState ** state)
//{
//	HRESULT hr;
//
//	hr = D3D::GetDevice()->CreateSamplerState(desc, state);
//	assert(SUCCEEDED(hr));
//}
//
//void States::GetBlendDesc(D3D11_BLEND_DESC * desc)
//{
//	memcpy(desc, &blendDesc, sizeof(D3D11_BLEND_DESC));
//}
//
//void States::CreateBlend(D3D11_BLEND_DESC * desc, ID3D11BlendState ** state)
//{
//	HRESULT hr;
//
//	hr = D3D::GetDevice()->CreateBlendState(desc, state);
//	assert(SUCCEEDED(hr));
//}
//
//void States::CreateRasterizerDesc()
//{
//	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
//	rasterizerDesc.AntialiasedLineEnable = false;
//	rasterizerDesc.CullMode = D3D11_CULL_BACK;
//	rasterizerDesc.DepthBias = 0;
//	rasterizerDesc.DepthBiasClamp = 0.0f;
//	rasterizerDesc.DepthClipEnable = true;
//	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
//	rasterizerDesc.FrontCounterClockwise = false;
//	rasterizerDesc.MultisampleEnable = false;
//	rasterizerDesc.ScissorEnable = false;
//	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
//}
//
//void States::CreateDepthStencilDesc()
//{
//	depthStencilDesc.DepthEnable = true;
//	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
//
//	depthStencilDesc.StencilEnable = true;
//	depthStencilDesc.StencilReadMask = 0xFF;
//	depthStencilDesc.StencilWriteMask = 0xFF;
//
//	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
//	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//
//	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
//	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//}
//
//void States::CreateSamplerDesc()
//{
//	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
//	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//	samplerDesc.MaxAnisotropy = 1;
//	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
//	samplerDesc.MinLOD = 0;
//	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
//}
//
//void States::CreateBlendDesc()
//{
//	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
//
//	blendDesc.AlphaToCoverageEnable = false;
//	blendDesc.IndependentBlendEnable = false;
//
//	blendDesc.RenderTarget[0].BlendEnable = false;
//	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
//	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
//	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
//
//	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
//	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
//	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
//
//	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//}
#include "stdafx.h"
#include "States.h"

D3D11_RASTERIZER_DESC States::rasterizerDesc;
D3D11_DEPTH_STENCIL_DESC States::depthStencilDesc;
D3D11_SAMPLER_DESC States::samplerDesc;
D3D11_BLEND_DESC States::blendDesc;

ID3D11RasterizerState* States::rasterizerStates[RasterizerStates::RASTERIZER_END];
ID3D11DepthStencilState* States::depthStencilStates[DepthStencilStates::DEPTHSTENCIL_END];
ID3D11SamplerState* States::samplerStates[SamplerStates::SAMPLER_END];
ID3D11BlendState* States::blendStates[BlendStates::BLEND_END];

void States::Create()
{
	CreateRasterizerDesc();
	CreateDepthStencilDesc();
	CreateSamplerDesc();
	CreateBlendDesc();
}

void States::Delete()
{
	for (int i = 0; i < RasterizerStates::RASTERIZER_END; ++i)
		SAFE_RELEASE(rasterizerStates[i]);
	for (int i = 0; i < DepthStencilStates::DEPTHSTENCIL_END; ++i)
		SAFE_RELEASE(depthStencilStates[i]);
	for (int i = 0; i < SamplerStates::SAMPLER_END; ++i)
		SAFE_RELEASE(samplerStates[i]);
	for (int i = 0; i < BlendStates::BLEND_END; ++i)
		SAFE_RELEASE(blendStates[i]);
}

void States::GetRasterizerDesc(D3D11_RASTERIZER_DESC* desc)
{
	memcpy(desc, &rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
}

void States::CreateRasterizer(D3D11_RASTERIZER_DESC* desc, ID3D11RasterizerState** state)
{
	HRESULT hr;

	hr = D3D::GetDevice()->CreateRasterizerState(desc, state);
	assert(SUCCEEDED(hr));
}

void States::SetRasterizer(ID3D11RasterizerState * state)
{
	D3D::GetDC()->RSSetState(state);
}

void States::SetRasterizer(RasterizerStates state)
{
	D3D::GetDC()->RSSetState(rasterizerStates[state]);
}

void States::GetDepthStencilDesc(D3D11_DEPTH_STENCIL_DESC* desc)
{
	memcpy(desc, &depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
}

void States::CreateDepthStencil(D3D11_DEPTH_STENCIL_DESC * desc, ID3D11DepthStencilState ** state)
{
	HRESULT hr;

	hr = D3D::GetDevice()->CreateDepthStencilState(desc, state);
	assert(SUCCEEDED(hr));
}

void States::SetDepthStencil(ID3D11DepthStencilState * state)
{
	D3D::GetDC()->OMSetDepthStencilState(state, 1);
}

void States::SetDepthStencil(DepthStencilStates state)
{
	D3D::GetDC()->OMSetDepthStencilState(depthStencilStates[state], 1);
}

void States::GetSamplerDesc(D3D11_SAMPLER_DESC * desc)
{
	memcpy(desc, &samplerDesc, sizeof(D3D11_SAMPLER_DESC));
}

void States::CreateSampler(D3D11_SAMPLER_DESC * desc, ID3D11SamplerState ** state)
{
	HRESULT hr;

	hr = D3D::GetDevice()->CreateSamplerState(desc, state);
	assert(SUCCEEDED(hr));
}

void States::SetSampler(UINT slot, UINT count, ID3D11SamplerState * state)
{
	D3D::GetDC()->PSSetSamplers(slot, count, &state);
}

void States::SetSampler(UINT slot, SamplerStates state)
{
	D3D::GetDC()->PSSetSamplers(slot, 1, &samplerStates[state]);
}

ID3D11SamplerState * States::GetSampler(SamplerStates state)
{
	return samplerStates[state];
}

void States::GetBlendDesc(D3D11_BLEND_DESC * desc)
{
	memcpy(desc, &blendDesc, sizeof(D3D11_BLEND_DESC));
}

void States::CreateBlend(D3D11_BLEND_DESC * desc, ID3D11BlendState ** state)
{
	HRESULT hr;
	hr = D3D::GetDevice()->CreateBlendState(desc, state);
	assert(SUCCEEDED(hr));
}

void States::SetBlend(ID3D11BlendState * state)
{
	D3D::GetDC()->OMSetBlendState(state, NULL, 0xFFFFFF);
}

void States::SetBlend(BlendStates state)
{
	D3D::GetDC()->OMSetBlendState(blendStates[state], NULL, 0xFFFFFF);
}

void States::CreateRasterizerDesc()
{
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	D3D11_RASTERIZER_DESC desc = rasterizerDesc;
	States::CreateRasterizer(&desc, &rasterizerStates[SOLID_CULL_ON]);

	desc.FrontCounterClockwise = true;
	States::CreateRasterizer(&desc, &rasterizerStates[SOLID_CCW]);
	desc.FrontCounterClockwise = false;

	desc.CullMode = D3D11_CULL_NONE;
	States::CreateRasterizer(&desc, &rasterizerStates[SOLID_CULL_OFF]);

	desc.FillMode = D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_BACK;
	States::CreateRasterizer(&desc, &rasterizerStates[WIRE_CULL_ON]);

	desc.FrontCounterClockwise = true;
	States::CreateRasterizer(&desc, &rasterizerStates[WIRE_CCW]);
	desc.FrontCounterClockwise = false;

	desc.CullMode = D3D11_CULL_NONE;
	States::CreateRasterizer(&desc, &rasterizerStates[WIRE_CULL_OFF]);
}

void States::CreateDepthStencilDesc()
{
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	D3D11_DEPTH_STENCIL_DESC desc = depthStencilDesc;
	States::CreateDepthStencil(&desc, &depthStencilStates[DEPTH_ON]);

	desc.DepthEnable = false;
	States::CreateDepthStencil(&desc, &depthStencilStates[DEPTH_OFF]);
}

void States::CreateSamplerDesc()
{
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	D3D11_SAMPLER_DESC desc = samplerDesc;
	States::CreateSampler(&desc, &samplerStates[LINEAR]);

	desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	States::CreateSampler(&desc, &samplerStates[LINEAR_MIRROR]);

	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	States::CreateSampler(&desc, &samplerStates[LINEAR_WRAP]);

	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	States::CreateSampler(&desc, &samplerStates[POINT_WRAP]);

	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	States::CreateSampler(&desc, &samplerStates[POINT]);

	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.BorderColor[0] = 0.0f; desc.BorderColor[1] = 0.0f;
	desc.BorderColor[2] = 0.0f; desc.BorderColor[3] = 0.0f;

	States::CreateSampler(&desc, &samplerStates[POINT_BORDER]);

	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	States::CreateSampler(&desc, &samplerStates[LINEAR_BORDER]);
}

void States::CreateBlendDesc()
{
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable = false; //DX11���� �߰��� ���
											 //����Ÿ�� 0~7������ 8���� ����� �� �ִµ�, 
											 //IndependentBlendEnable�� true�� �ϸ� �� ����Ÿ�ٸ��� ���������� ���� ������ �����ϴ�.
											 //������ false�� ��� 0�� ����Ÿ�ٿ� ������ �������� ��� ����Ÿ�ٿ� �����ȴ�.
	blendDesc.IndependentBlendEnable = false;

	//���ĺ���� �⺻������ ������ RenderTarget�� ����ȴ�.
	//���ĺ��� ���귮�� ����� ����. �׷��� �ʿ���� �κ��� �ݵ�� ����.
	blendDesc.RenderTarget[0].BlendEnable = false;

	//�⺻���� ���� ������ �־���´�.
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	//���İ��� �� �ȼ��� 0.5, 0.5, 0.5, 0.0f �̷��� ������ ���
	//ALL�� rgb ���δ� ���ڴٴ� ���̴�. ���� RED�� ���� ��� R���� ������.
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC desc = blendDesc;
	States::CreateBlend(&desc, &blendStates[BlendStates::BLENDING_OFF]);

	desc.RenderTarget[0].BlendEnable = true;
	States::CreateBlend(&desc, &blendStates[BlendStates::BLENDING_ON]);
}
