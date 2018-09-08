#include "stdafx.h"
#include "GrassRenderer.h"

#include "./Utilities/Buffer.h"

GrassRenderer::GrassRenderer(UINT maxGrassCount)
{
	this->CreateBuffer(maxGrassCount);
}

GrassRenderer::~GrassRenderer()
{
}

void GrassRenderer::UpdateBuffer()
{
	Buffer::UpdateBuffer(&vertexBuffer, &grassData[0], sizeof VertexGrass * maxGrassCount);
}

void GrassRenderer::Render()
{

}

void GrassRenderer::UIRender()
{

}

void GrassRenderer::AddGrass(VertexGrass grass)
{

}

void GrassRenderer::CreateBuffer(UINT grassCount)
{
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = (sizeof VertexGrass) * grassCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	HRESULT hr = Device->CreateBuffer(&desc, nullptr, &vertexBuffer);
	assert(SUCCEEDED(hr));
}
