#include "stdafx.h"
#include "BloomEffect.h"

#include "./ComputeShader/ComputeShader.h"
#include "./ComputeShader/ComputeResource.h"

#include "./Renders/DeferredRenderer.h"

BloomEffect::BloomEffect()
	:deferredRenderingSRV(nullptr)
{
	this->InitailizeValue();
	this->CreateShaders();
	this->CreateBuffers();

	RenderRequest->AddRender("BloomUIRender", bind(&BloomEffect::UIRender, this), RenderType::UIRender);
}


BloomEffect::~BloomEffect()
{
	SafeDelete(downScaleFirstShader);
	SafeDelete(downScaleSecondShader);
	SafeDelete(bloomShader);
	SafeDelete(verticalBlurShader);
	SafeDelete(horizentalBlurShader);
	SafeDelete(finalRenderingShader);

	SafeDelete(downScaleBuffer);
	SafeDelete(finalRenderingBuffer);
	SafeDelete(downTextureBuffer);
	SafeDelete(averageValuesBuffer);
	SafeDelete(prevAvgLumBuffer);
	SafeDelete(downScale1DBuffer);
	SafeDelete(bloomTextureBuffer);
	SafeDelete(temporaryTexture0);
	SafeDelete(temporaryTexture1);
}


void BloomEffect::SetRTV()
{
}

void BloomEffect::UIRender()
{
	ImGui::Begin("BloomEffect");

	ImGui::Checkbox("HDRActive", reinterpret_cast<bool*>(&finalRenderingBuffer->data.hdrActive));
	ImGui::SameLine();
	ImGui::Checkbox("BloomActive", reinterpret_cast<bool*>(&finalRenderingBuffer->data.bloomActive));

	ID3D11ShaderResourceView* view = temporaryTexture0->GetSRV();
	ImGui::ImageButton(view, ImVec2(100, 100));

	ImGui::SliderFloat("MiddleGrey", &middleGrey, 0.0f, 50.0f);
	ImGui::SliderFloat("White", &white, 0.0f, 10.0f);
	ImGui::SliderFloat("BloomTreshold", &bloomThreshold, 0.0f, 10.0f);
	ImGui::SliderFloat("BloomScale", &bloomScale, 0.0f, 100.0f);
	ImGui::SliderFloat("Adaption", &adaption, 0.0f, 10.0f);

	ImGui::End();

}



void BloomEffect::Render()
{
	this->BindConstantBuffer();
	this->DonwScale();
	//this->Bloom();
	//this->Blur();
	this->FinalRendering();
}

void BloomEffect::InitailizeValue()
{
	this->width = WinSizeX;
	this->height = WinSizeY;
	this->downScaleGroup = (UINT)ceil((float)(width * height / 16) / 1024.0f);

	this->middleGrey = 9.76f;
	this->white = 0.46f;
	this->bloomThreshold = 1.1f;
	this->bloomScale = 0.74f;
	this->adaption = 1.0f;
}

void BloomEffect::CreateShaders()
{
	this->downScaleFirstShader = new ComputeShader(ShaderPath + L"999_DownScaleCompute.hlsl", "CSMain0");
	this->downScaleSecondShader = new ComputeShader(ShaderPath + L"999_DownScaleCompute.hlsl", "CSMain1");
	this->bloomShader = new ComputeShader(ShaderPath + L"999_BloomCompute.hlsl", "CSMain");
	this->verticalBlurShader = new ComputeShader(ShaderPath + L"999_BlurCompute.hlsl", "VerticalFilter");
	this->horizentalBlurShader = new ComputeShader(ShaderPath + L"999_BlurCompute.hlsl", "HorizFilter");
	this->finalRenderingShader = new Shader(ShaderPath + L"999_Bloom.hlsl");
}

void BloomEffect::CreateBuffers()
{
	this->downScaleBuffer = new DonwScaleBuffer;
	this->finalRenderingBuffer = new FinalRenderingBuffer;
	this->averageValuesBuffer = new CResource1D(sizeof(float), 1, nullptr);
	this->prevAvgLumBuffer = new CResource1D(sizeof(float), 1, nullptr);
	this->downScale1DBuffer = new CResource1D(sizeof(float), downScaleGroup, nullptr);
	this->downTextureBuffer = new CResource2D(WinSizeX / 4, WinSizeY / 4, DXGI_FORMAT_R16G16B16A16_FLOAT);
	this->bloomTextureBuffer = new CResource2D(WinSizeX / 4, WinSizeY / 4, DXGI_FORMAT_R16G16B16A16_FLOAT);
	this->temporaryTexture0 = new CResource2D(WinSizeX / 4, WinSizeY / 4, DXGI_FORMAT_R16G16B16A16_FLOAT);
	this->temporaryTexture1 = new CResource2D(WinSizeX / 4, WinSizeY / 4, DXGI_FORMAT_R16G16B16A16_FLOAT);

}

void BloomEffect::BindConstantBuffer()
{
	downScaleBuffer->data.width = WinSizeX / 4;
	downScaleBuffer->data.height = WinSizeY / 4;
	downScaleBuffer->data.totalPixels = downScaleBuffer->data.width * downScaleBuffer->data.height;
	downScaleBuffer->data.groupSize = downScaleGroup;
	downScaleBuffer->data.adaptation = adaption;
	downScaleBuffer->data.bloomThreshold = bloomThreshold;

	downScaleBuffer->SetCSBuffer(1);
}

void BloomEffect::DonwScale()
{
	//nullRenderTarget Binding - 랜더타겟으로 작성한 SRV가 완성되는 시점은 바인딩 해제되었을 때 이므로 반드시 언바인딩을 해주자.
	ID3D11RenderTargetView* nullRenderTarget = nullptr;
	DeviceContext->OMSetRenderTargets(1, &nullRenderTarget, nullptr);

	//DownScale 1 Pass ------------------------------------------------------
	this->downScale1DBuffer->BindResource(0);
	this->downTextureBuffer->BindResource(1);
	DeviceContext->CSSetShaderResources(0, 1, &deferredRenderingSRV);

	this->downScaleFirstShader->BindShader();
	this->downScaleFirstShader->Dispatch(this->downScaleGroup, 1, 1);
	//------------------------------------------------------------------
	downScale1DBuffer->ReleaseResource(0);
	downTextureBuffer->ReleaseResource(1);
	ID3D11ShaderResourceView* nullView[1] = { nullptr };
	DeviceContext->CSSetShaderResources(0, 1, nullView);
	//DownScale 2 Pass ---------------------------------------------------------
	this->averageValuesBuffer->BindResource(0);
	
	this->downScale1DBuffer->BindCSShaderResourceView(1);
	this->prevAvgLumBuffer->BindCSShaderResourceView(2);

	this->downScaleSecondShader->BindShader();
	this->downScaleSecondShader->Dispatch(1, 1, 1);
	//-------------------------------------------------------------------

	this->averageValuesBuffer->ReleaseResource(0);
	ID3D11ShaderResourceView* arrView[3] = { nullptr,nullptr,nullptr };
	DeviceContext->VSSetShaderResources(0, 3, arrView);
}

void BloomEffect::Bloom()
{
	this->downTextureBuffer->BindCSShaderResourceView(0);
	this->averageValuesBuffer->BindCSShaderResourceView(1);
	this->temporaryTexture0->BindResource(0);

	this->bloomShader->BindShader();
	this->bloomShader->Dispatch(this->downScaleGroup, 1, 1);

	downTextureBuffer->ReleaseCSshaderResorceView(0);
	averageValuesBuffer->ReleaseCSshaderResorceView(1);
	temporaryTexture0->ReleaseResource(0);

	DeviceContext->CSSetShader(NULL, NULL, 0);
}

void BloomEffect::Blur()
{
	this->downScaleBuffer->SetCSBuffer(1);

	//Horizental Computing 
	this->temporaryTexture1->BindResource(0);
	this->temporaryTexture0->BindCSShaderResourceView(1);
	this->horizentalBlurShader->BindShader();
	this->horizentalBlurShader->Dispatch((UINT)ceil((width / 4.0f) / (128.0f - 12.0f)), (UINT)ceil(height / 4.0f), 1);

	temporaryTexture1->ReleaseResource(0);
	temporaryTexture0->ReleaseCSshaderResorceView(0);

	DeviceContext->CSSetShader(NULL, NULL, 0);

	//Vertical Computing
	this->bloomTextureBuffer->BindResource(0);
	this->temporaryTexture1->BindCSShaderResourceView(0);
	this->verticalBlurShader->BindShader();
	this->verticalBlurShader->Dispatch((UINT)ceil(width / 4.0f), (UINT)ceil((height / 4.0f) / (128.0f - 12.0f)), 1);

	bloomTextureBuffer->ReleaseResource(0);
	temporaryTexture1->ReleaseCSshaderResorceView(0);
	DeviceContext->CSSetShader(NULL, NULL, 0);
}

void BloomEffect::FinalRendering()
{
	pRenderer->ChangeZBuffer(false);
	pRenderer->BeginDraw();

	finalRenderingBuffer->data.middleGrey = middleGrey;
	finalRenderingBuffer->data.lumWhiteSqr = white;
	finalRenderingBuffer->data.lumWhiteSqr *= middleGrey;
	finalRenderingBuffer->data.lumWhiteSqr *= finalRenderingBuffer->data.lumWhiteSqr;
	finalRenderingBuffer->data.bloomScale = bloomScale;

	finalRenderingBuffer->SetPSBuffer(1);

	States::SetSampler(3, States::SamplerStates::POINT_WRAP);
	States::SetSampler(4, States::SamplerStates::LINEAR_WRAP);

	DeviceContext->PSSetShaderResources(0, 1, &this->deferredRenderingSRV);
	this->averageValuesBuffer->BindPSShaderResourceView(1);
	this->temporaryTexture0->BindPSShaderResourceView(2);

	DeviceContext->IASetInputLayout(nullptr);
	DeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	DeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	finalRenderingShader->Render();
	DeviceContext->Draw(4, 0);

	CResource1D* temp = averageValuesBuffer;
	averageValuesBuffer = prevAvgLumBuffer;
	prevAvgLumBuffer = temp;

	ID3D11ShaderResourceView* arrView[3] = { nullptr,nullptr,nullptr };
	DeviceContext->PSSetShaderResources(0, 3, arrView);
	pRenderer->ChangeZBuffer(true);

}
