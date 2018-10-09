//#pragma once
//
//class States
//{
//public:
//	static void Create();
//	static void Delete();
//
//	static void GetRasterizerDesc(D3D11_RASTERIZER_DESC* desc);
//	static void CreateRasterizer(D3D11_RASTERIZER_DESC* desc, ID3D11RasterizerState** state);
//
//	static void GetDepthStencilDesc(D3D11_DEPTH_STENCIL_DESC* desc);
//	static void CreateDepthStencil(D3D11_DEPTH_STENCIL_DESC* desc, ID3D11DepthStencilState** state);
//
//	static void GetSamplerDesc(D3D11_SAMPLER_DESC* desc);
//	static void CreateSampler(D3D11_SAMPLER_DESC* desc, ID3D11SamplerState** state);
//
//	static void GetBlendDesc(D3D11_BLEND_DESC* desc);
//	static void CreateBlend(D3D11_BLEND_DESC* desc, ID3D11BlendState** state);
//
//private:
//	static void CreateRasterizerDesc();
//	static void CreateDepthStencilDesc();
//	static void CreateSamplerDesc();
//	static void CreateBlendDesc();
//
//	static D3D11_RASTERIZER_DESC rasterizerDesc;
//	static D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
//	static D3D11_SAMPLER_DESC samplerDesc;
//	static D3D11_BLEND_DESC blendDesc;
//};
#pragma once

class States
{
public:
	enum RasterizerStates {
		SOLID_CULL_ON, SOLID_CULL_OFF, SOLID_CCW,
		WIRE_CULL_ON, WIRE_CULL_OFF, WIRE_CCW,SHADOW, RASTERIZER_END
	};
	enum DepthStencilStates { DEPTH_ON, DEPTH_OFF, DEPTHSTENCIL_END };
	enum SamplerStates { LINEAR, POINT, LINEAR_WRAP, LINEAR_MIRROR, POINT_WRAP, LINEAR_BORDER, POINT_BORDER,
		SAMPLE_LEVELZERO,SAMPLER_END };
	enum BlendStates { BLENDING_ON, BLENDING_OFF, BLEND_END };

	static void Create();
	static void Delete();

	static void GetRasterizerDesc(D3D11_RASTERIZER_DESC* desc);
	static void CreateRasterizer(D3D11_RASTERIZER_DESC* desc, ID3D11RasterizerState** state);
	static void SetRasterizer(ID3D11RasterizerState* state);
	static void SetRasterizer(RasterizerStates state);

	static void GetDepthStencilDesc(D3D11_DEPTH_STENCIL_DESC* desc);
	static void CreateDepthStencil(D3D11_DEPTH_STENCIL_DESC* desc, ID3D11DepthStencilState** state);
	static void SetDepthStencil(ID3D11DepthStencilState* state);
	static void SetDepthStencil(DepthStencilStates state);

	static void GetSamplerDesc(D3D11_SAMPLER_DESC* desc);
	static void CreateSampler(D3D11_SAMPLER_DESC* desc, ID3D11SamplerState** state);
	static void SetSampler(UINT slot, UINT count, ID3D11SamplerState* state);
	static void SetSampler(UINT slot, SamplerStates state);
	static ID3D11SamplerState* GetSampler(SamplerStates state);

	static void GetBlendDesc(D3D11_BLEND_DESC* desc);
	static void CreateBlend(D3D11_BLEND_DESC* desc, ID3D11BlendState** state);
	static void SetBlend(ID3D11BlendState* state);
	static void SetBlend(BlendStates state);

private:
	static void CreateRasterizerDesc();
	static void CreateDepthStencilDesc();
	static void CreateSamplerDesc();
	static void CreateBlendDesc();

	static D3D11_RASTERIZER_DESC rasterizerDesc;
	static D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	static D3D11_SAMPLER_DESC samplerDesc;
	static D3D11_BLEND_DESC blendDesc;

	static ID3D11RasterizerState* rasterizerStates[RasterizerStates::RASTERIZER_END];
	static ID3D11DepthStencilState* depthStencilStates[DepthStencilStates::DEPTHSTENCIL_END];
	static ID3D11SamplerState* samplerStates[SamplerStates::SAMPLER_END];
	static ID3D11BlendState* blendStates[BlendStates::BLEND_END];
};