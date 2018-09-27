#include "stdafx.h"
#include "GrassRenderer.h"

#include "./Utilities/Buffer.h"


GrassRenderer::GrassRenderer(UINT maxGrass)
	:maxGrassCount(maxGrass)
{
	//shader = new Shader();

	for(UINT i =0; i < 10;++i)
		nullView[i] = nullptr;
}

GrassRenderer::~GrassRenderer()
{
}

void GrassRenderer::Init()
{
}

void GrassRenderer::Release()
{
	SafeRelease(grassBuffer);
	
	grassList.clear();
	textureList.clear();
}

void GrassRenderer::PreUpdate()
{
}

void GrassRenderer::Update()
{
}

void GrassRenderer::PostUpdate()
{
}

void GrassRenderer::PrevRender()
{
}

void GrassRenderer::Render()
{
	//Bind ---------------------------------------------------------------
	States::SetRasterizer(States::RasterizerStates::SOLID_CULL_OFF);

	ID3D11ShaderResourceView* view[10];
	for (UINT i = 0; i < 10; ++i)
	{
		view[i] = nullptr;
		if (i < textureList.size())
			view[i] = textureList[i]->GetSRV();
	}
	// -------------------------------------------------------------------

	DeviceContext->PSSetShaderResources(0, 10, view);

	UINT stride = sizeof GrassData;
	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &grassBuffer, &stride, &offset);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	shader->Render();

	DeviceContext->Draw(grassList.size(), 0);

	DeviceContext->PSSetShaderResources(0, 10, nullView);
	States::SetRasterizer(States::RasterizerStates::SOLID_CULL_ON);

}

void GrassRenderer::UIRender()
{
}

void GrassRenderer::ShadowRender()
{
}

void GrassRenderer::AddGrass(GrassData data)
{
	grassList.push_back(data);
	this->UpdateBuffer();
}

void GrassRenderer::CreateBuffer(bool mapTool /*== true*/)
{
	if (mapTool)
		Buffer::CreateDynamicVertexBuffer(&grassBuffer, nullptr, sizeof GrassData *  maxGrassCount);
	else
		Buffer::CreateVertexBuffer(&grassBuffer, grassList.data(), sizeof GrassData * grassList.size());
}

void GrassRenderer::UpdateBuffer()
{
	Buffer::UpdateBuffer(&grassBuffer, grassList.data(), sizeof GrassData * grassList.size());
}
