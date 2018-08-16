#include "stdafx.h"
#include "DepthVis.h"
#include "./ComputeShader/ComputeShader.h"
#include "./ComputeShader/ComputeResource.h"


DepthVis::DepthVis()
{
	shader = new ComputeShader(ShaderPath + L"DepthVis.hlsl");

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);

	depth = new CResource2D(desc.Width, desc.Height);
	
}


DepthVis::~DepthVis()
{
}

void DepthVis::CalcuDepth(ID3D11ShaderResourceView * depthView)
{
	ID3D11ShaderResourceView* nullView[1] = {nullptr};
	shader->BindShader();
	DeviceContext->CSSetShaderResources(0, 1, &depthView);
	depth->BindResource(0);
	shader->Dispatch(80, 20, 1);


	depth->ReleaseResource(0);
	DeviceContext->CSSetShaderResources(0, 1, nullView);
	
}

ID3D11ShaderResourceView * DepthVis::GetSRV()
{
	return depth->GetSRV();
}
