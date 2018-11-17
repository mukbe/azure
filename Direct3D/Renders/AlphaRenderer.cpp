#include "stdafx.h"
#include "AlphaRenderer.h"

#include "./Figure/Figure.h"
#include "./Utilities/Transform.h"


AlphaRenderer::AlphaRenderer()
	:renderTargetView(nullptr),depthView(nullptr)
{
	
}


AlphaRenderer::~AlphaRenderer()
{
	
}

void AlphaRenderer::SetRTV()
{
	DeviceContext->OMSetRenderTargets(1, &renderTargetView, depthView);
}

void AlphaRenderer::Render()
{
	States::SetBlend(States::BlendStates::BLENDING_OFF);
	DeviceContext->OMSetRenderTargets(1, &renderTargetView, pRenderer->GetDepthStencilView());
}

void AlphaRenderer::UIRender()
{

}
