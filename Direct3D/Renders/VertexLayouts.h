#pragma once

//=======================================================================================

struct Vertex
{
	Vertex()
		: position(0, 0, 0) {}
	Vertex(D3DXVECTOR3 pos)
		:position(pos) {}

	D3DXVECTOR3 position;
};

//=======================================================================================

struct VertexColor
{
	VertexColor()
		: position(0, 0, 0)
		, color(0, 0, 0, 1) {}
	VertexColor(D3DXVECTOR3 pos, D3DXCOLOR col)
		: position(pos), color(col) {}

	D3DXVECTOR3	position;
	D3DXCOLOR color;
};

//=======================================================================================

struct VertexColorNormal
{
	VertexColorNormal()
		: position(0, 0, 0)
		, color(0, 0, 0, 1)
		, normal(0, 0, 0) {}

	D3DXVECTOR3 position;
	D3DXCOLOR color;
	D3DXVECTOR3 normal;
};

//=======================================================================================

struct VertexTexture
{
	VertexTexture()
		: position(0, 0, 0)
		, uv(0, 0) {}

	D3DXVECTOR3	position;
	D3DXVECTOR2	uv;
};

//=======================================================================================

struct VertexTextureNormal
{
	VertexTextureNormal()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0) {}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;
};

//=======================================================================================

struct VertexColorTextureNormal
{
	VertexColorTextureNormal()
		: position(0, 0, 0)
		, color(0, 0, 0, 1)
		, uv(0, 0)
		, normal(0, 0, 0) {}

	D3DXVECTOR3 position;
	D3DXCOLOR color;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;
};

//=======================================================================================

struct VertexTextureNormalBlend
{
	VertexTextureNormalBlend()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0)
		, blendIndices(0, 0, 0, 0)
		, blendWeights(0, 0, 0, 0) {}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;
	D3DXVECTOR4 blendIndices;
	D3DXVECTOR4 blendWeights;
};

//=======================================================================================

struct VertexTextureNormalTangent
{
	VertexTextureNormalTangent()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0)
		, tangent(0, 0, 0)
	{}

	D3DXVECTOR3	position;
	D3DXVECTOR2	uv;
	D3DXVECTOR3	normal;
	D3DXVECTOR3 tangent;
};

//========================================================================================

struct VertexTextureBlendNT
{
	VertexTextureBlendNT()
		: position(0, 0, 0)
		, uv(0, 0)
		, blendIndices(0, 0, 0, 0)
		, blendWeights(0, 0, 0, 0) 
		, normal(0, 0, 0)
		, tangent(0, 0, 0)

	{}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXVECTOR4 blendIndices;
	D3DXVECTOR4 blendWeights;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 tangent;

};

struct VertexParticle
{
	VertexParticle()
		: position(0, 0, 0)
		, uv(0, 0)
		, color(0, 0, 0, 0)
		, normal(0, 0, 0)
		, scale(0, 0)

	{}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXVECTOR4 color;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 scale;

};
