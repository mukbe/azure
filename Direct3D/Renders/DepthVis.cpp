#include "stdafx.h"
#include "DepthVis.h"
#include "./ComputeShader/ComputeShader.h"
#include "./ComputeShader/ComputeResource.h"


DepthVis::DepthVis(ID3D11ShaderResourceView* depthView)
{
	shader = new ComputeShader(ShaderPath + L"DepthVis.hlsl");

	depth = new CResource2D()
}


DepthVis::~DepthVis()
{
}
