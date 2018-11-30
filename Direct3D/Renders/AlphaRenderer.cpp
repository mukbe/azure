#include "stdafx.h"
#include "AlphaRenderer.h"

#include "./Figure/Figure.h"
#include "./Utilities/Transform.h"


AlphaRenderer::AlphaRenderer()
	:renderTargetView(nullptr),depthView(nullptr)
{
	for (UINT i = 0; i < 50; ++i)
	{
		Figure* newFigure = new Figure(Figure::FigureType::Box, 10.0f, Math::RandomColor());
		newFigure->GetTransform()->SetWorldPosition(Math::Random(0.f, 256.f), Math::Random(0.f, 30.0f), Math::Random(0.f, 256.0f));
		this->figures.push_back(newFigure);
	}
}


AlphaRenderer::~AlphaRenderer()
{
	
}

void AlphaRenderer::SetRTV()
{
	DeviceContext->OMSetRenderTargets(1, &renderTargetView, depthView);
	States::SetBlend(States::BlendStates::BLENDING_ON);
	for (UINT i = 0; i < figures.size(); ++i)
		figures[i]->Render();
}

void AlphaRenderer::Render()
{
	States::SetBlend(States::BlendStates::BLENDING_OFF);
	DeviceContext->OMSetRenderTargets(1, &renderTargetView, pRenderer->GetDepthStencilView());

}

void AlphaRenderer::UIRender()
{
	
}
