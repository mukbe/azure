#include "stdafx.h"
#include "Ocean.h"

#include "./Utilities/Buffer.h"
#include "./Renders/Texture.h"
#include "./Renders/Instancing/InstanceShader.h"
#include "./ComputeShader/ComputeShader.h"
#include "./ComputeShader/ComputeResource.h"

#include "./Utilities/Math.h"

#define Gravity 9.81f

void Ocean::ComputingOcean()
{
	//BindBuffers --------------------------------------------
	this->oceanComputeBuffer->SetCSBuffer(5);

	this->rwHeightBuffer->BindResource(0);
	this->rwSlopeBuffer->BindResource(1);
	this->rwDisplacementBuffer->BindResource(2);
	this->spectrumBuffer->BindResource(3);
	this->spectrum_conjBuffer->BindResource(4);
	this->butterflyLookupTableBuffer->BindResource(5);
	this->dispersionTableBuffer->BindResource(6);
	this->vertexDataBuffer->BindResource(7);
	// -------------------------------------------------------
	//Start Computing -----------------------------------------
	computeSpectrumShader->BindShader();
	computeSpectrumShader->Dispatch(vertexLength, vertexLength, 1);

	for (UINT i = 0; i < passes; ++i)
	{
		oceanComputeBuffer->data.nowPass = i;
		oceanComputeBuffer->SetCSBuffer(5);
		peformFFTShader0->BindShader();
		peformFFTShader0->Dispatch(1, vertexLength, vertexLength);
	}
	for (UINT i = 0; i < passes; ++i)
	{
		oceanComputeBuffer->data.nowPass = i;
		oceanComputeBuffer->SetCSBuffer(5);
		peformFFTShader1->BindShader();
		peformFFTShader1->Dispatch(1, vertexLength, vertexLength);
	}

	this->originVertexBuffer->BindResource(3);
	updateVertexShader->BindShader();
	updateVertexShader->Dispatch(vertexLength, vertexLength, 1);

	//-----------------------------------------------------------
	//GetData --------------------------------------------------
	vertexDataBuffer->GetDatas(vertexData.data());
	//-----------------------------------------------------------
	//ReleaseBuffers -------------------------------------------
	ID3D11UnorderedAccessView* nullView[1] = { nullptr };
	for (UINT i = 0; i < 8; ++i)
		DeviceContext->CSSetUnorderedAccessViews(i, 1, nullView, nullptr);

	ID3D11Buffer* nullBuffer[1] = { nullptr };
	DeviceContext->CSSetConstantBuffers(5, 1, nullBuffer);
	//---------------------------------------------------------------
}

void Ocean::FrustumCulling()
{
}

D3DXVECTOR2 Ocean::GetSpectrum(int n_prime, int m_prime)
{
	D3DXVECTOR2 r = GaussianRandomVariable();
	return r * sqrtf(PhillipsSpectrum(n_prime, m_prime) / 2.0f);
}

D3DXVECTOR2 Ocean::GaussianRandomVariable()
{
	float x1, x2, w;
	do
	{
		x1 = 2.0f * Math::RandF() - 1.0f;
		x2 = 2.0f * Math::RandF() - 1.0f;
		w = x1 * x1 + x2 * x2;
	} while (w >= 1.0f);

	w = sqrtf((-2.0f * log(w)) / w);
	return D3DXVECTOR2(x1 * w, x2 * w);
}

float Ocean::Dispersion(int n_prime, int m_prime)
{
	float w_0 = 2.0f * D3DX_PI / 200.0f;
	float kx = D3DX_PI * (2 * n_prime - vertexLength) / length;
	float kz = D3DX_PI * (2 * m_prime - vertexLength) / length;
	return (float)(((int)(sqrtf(Gravity * sqrtf(kx * kx + kz * kz)) / w_0)) * w_0);
}

float Ocean::PhillipsSpectrum(int n_prime, int m_prime)
{
	D3DXVECTOR2 k = D3DXVECTOR2(D3DX_PI * (2 * n_prime - vertexLength) / length, D3DX_PI * (2 * m_prime - vertexLength) / length);
	float k_length = D3DXVec2Length(&k);
	if (k_length < 0.000001f) return 0.0f;

	float k_length2 = k_length * k_length;
	float k_length4 = k_length2 * k_length2;

	D3DXVec2Normalize(&k, &k);

	float k_dot_w = D3DXVec2Dot(&k, &windDirection);
	float k_dot_w2 = k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w;

	float w_length = D3DXVec2Length(&windSpeed);
	float L = w_length * w_length / Gravity;
	float L2 = L * L;

	float damping = 0.001f;
	float l2 = L2 * damping * damping;

	return waveAmp * exp(-1.0f / (k_length2 * L2)) / k_length4 * k_dot_w2 * exp(-k_length2 * l2);
}

void Ocean::CreateFresnelLookUpTable()
{
	float nSnell = 1.34f;

	fresnelLookUp = new Texture();
	vector<D3DXCOLOR> colors;
	colors.assign(512, D3DXCOLOR());

	for (int x = 0; x < 512; x++)
	{
		float fresnel = 0.0f;
		float costhetai = (float)x / 511.0f;
		float thetai = acosf(costhetai);
		float sinthetat = sinf(thetai) / nSnell;
		float thetat = asinf(sinthetat);

		if (thetai == 0.0f)
		{
			fresnel = (nSnell - 1.0f) / (nSnell + 1.0f);
			fresnel = fresnel * fresnel;
		}
		else
		{
			float fs = sinf(thetat - thetai) / sinf(thetat + thetai);
			float ts = tanf(thetat - thetai) / tanf(thetat + thetai);
			fresnel = 0.5f * (fs*fs + ts * ts);
		}

		colors[x] = D3DXCOLOR(fresnel, fresnel, fresnel, fresnel);
	}

	this->fresnelLookUp->SetPixel(colors, 512, 1);

	colors.clear();
}

int Ocean::BitReverse(int i)
{
	int j = i;
	int Sum = 0;
	int W = 1;
	int M = vertexLength / 2;
	while (M != 0)
	{
		j = ((i & M) > M - 1) ? 1 : 0;
		Sum += j * W;
		W *= 2;
		M /= 2;
	}
	return Sum;
}
