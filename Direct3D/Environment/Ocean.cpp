#include "stdafx.h"
#include "Ocean.h"

#include "./Utilities/Buffer.h"
#include "./Renders/Texture.h"
#include "./Renders/Instancing/InstanceShader.h"
#include "./ComputeShader/ComputeShader.h"
#include "./ComputeShader/ComputeResource.h"
#include "./Renders/Material.h"

#include "./View/FreeCamera.h"


Ocean::Ocean()
{
	this->name = "Ocean";
	this->vertexLength = pow(2, 6);
	
	this->InitInstanceShader();
	this->InitOceansData();
	this->CreateFresnelLookUpTable();
	this->InitBuffers();
}


Ocean::~Ocean()
{
	SafeDelete(originVertexBuffer);
	SafeDelete(vertexDataBuffer);

	SafeDelete(butterflyLookupTableBuffer);
	SafeDelete(dispersionTableBuffer);
	SafeDelete(spectrum_conjBuffer);
	SafeDelete(spectrumBuffer);

	SafeDelete(rwDisplacementBuffer);
	SafeDelete(rwSlopeBuffer);
	SafeDelete(rwHeightBuffer);

	SafeDelete(oceanComputeBuffer);
	SafeDelete(updateVertexShader);
	SafeDelete(peformFFTShader1);
	SafeDelete(peformFFTShader0);
	SafeDelete(computeSpectrumShader);

	SafeRelease(vertexBuffer);
	SafeRelease(instanceBuffer);
	SafeRelease(indexBuffer);

	vertexData.clear();
	instanceData.clear();
	indexData.clear();

	SafeDelete(fresnelLookUp);
	SafeDelete(material);
	SafeDelete(instanceShader);
}

void Ocean::Update()
{
	oceanComputeBuffer->data.time += Time::Delta();

	this->ComputingOcean();
	this->UpdateBuffer();
	
}

void Ocean::Render()
{
	//BindBuffers ----------------------------------------------
	ID3D11ShaderResourceView* srv = fresnelLookUp->GetSRV();
	DeviceContext->VSSetShaderResources(6,1, &srv);
	//DeviceContext->PSSetShaderResources(6,1, &srv);
	material->SetDiffuseColor(oceanColor);
	material->UpdateBuffer();
	material->BindBuffer();
	worldBuffer->SetMatrix(transform->GetFinalMatrix());
	worldBuffer->SetVSBuffer(1);
	
	// -----------------------------------------------------------

	//Draw ----------------------------------------------------------
	UINT stride[2] = { sizeof VertexTextureNormal,sizeof D3DXVECTOR2 };
	UINT offset[2] = { 0 ,0 };
	ID3D11Buffer* buffers[2] = { vertexBuffer,instanceBuffer };

	DeviceContext->IASetVertexBuffers(0, 2, buffers, stride, offset);
	DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	instanceShader->Render();

	DeviceContext->DrawIndexedInstanced(indexData.size(), instanceData.size(), 0, 0, 0);
	//------------------------------------------------------------------------

	//ReleaseBuffer ---------------------------------------------------------
	ID3D11ShaderResourceView* nullSrv[1] = { nullptr };
	DeviceContext->PSSetShaderResources(6, 1, nullSrv);
	material->UnBindBuffer();

	//-----------------------------------------------------------------------
}

void Ocean::UIUpdate()
{
}

void Ocean::UIRender()
{
	ImGui::Text("GridX : %d , GridZ : %d", this->gridCountX,this->gridCountZ);
	ImGui::Text("FPS : %f", Time::Get()->FPS());

	ImGui::ColorEdit4("OceanColor", (float*)&oceanColor.r,
		ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreviewHalf);

	ImGui::Separator();
	
	transform->UIRender();

}

void Ocean::UpdateBuffer()
{
	Buffer::UpdateBuffer(&vertexBuffer, vertexData.data(), sizeof VertexTextureNormal * vertexData.size());
}


void Ocean::InitInstanceShader()
{
	D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[4] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,
		D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT ,0,12,
		D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT ,0,20,
		D3D11_INPUT_PER_VERTEX_DATA,0 },

		{ "TEXCOORD",1,DXGI_FORMAT_R32G32_FLOAT,1,0,
		D3D11_INPUT_PER_INSTANCE_DATA,1 },
	};
	
	instanceShader = new InstanceShader(ShaderPath + L"005_OceanRaw.hlsl", false);
	instanceShader->CreateInputLayout(inputLayoutDesc, 4);

	material = new Material;

	worldBuffer = Buffers->FindShaderBuffer<WorldBuffer>();
}

void Ocean::InitOceansData()
{
	this->gridCountX = 8;
	this->gridCountZ = 8;
	this->length = (float)vertexLength;
	this->waveAmp = 0.0002f;
	this->windSpeed = D3DXVECTOR2(32.0f, 32.0f);
	this->windDirection = D3DXVECTOR2(windSpeed.x, windSpeed.y);
	D3DXVec2Normalize(&windDirection, &windDirection);

	this->oceanColor = D3DXCOLOR(0.021f, 0.08f, 0.309f,1.0f);

}

void Ocean::InitBuffers()
{
	struct StructuredDataFloat2
	{
		D3DXVECTOR2 data;
		UINT indexX;
		UINT indexY;
		StructuredDataFloat2()
			:data(D3DXVECTOR2(0.f, 0.f)), indexX(0), indexY(0) {}
		StructuredDataFloat2(D3DXVECTOR2 data, UINT x, UINT y)
			:data(data), indexX(x), indexY(y) {}
	};

	struct StructuredDataFloat4
	{
		D3DXVECTOR4 data;
		UINT indexX;
		UINT indexY;
		StructuredDataFloat4()
			:data(D3DXVECTOR4(0.f, 0.f, 0.f, 0.f)), indexX(0), indexY(0) {}
		StructuredDataFloat4(D3DXVECTOR4 data, UINT x, UINT y)
			:data(data), indexX(x), indexY(y) {}
	};


	vector<StructuredDataFloat2>	heightData;
	vector<StructuredDataFloat4>	slopeData;
	vector<StructuredDataFloat4>	displacementData;
	vector<D3DXVECTOR2>				spectrum;
	vector<D3DXVECTOR2>				spectrum_conj;
	vector<float>					dispersionTable;
	vector<float>					butterflyLookupTable;
	vector<Vertex>					positionData;

	int		size = vertexLength;
	float	fSize = (float)size;
	this->passes = (int)(log(fSize) / log(2.0f));


	//푸리에 변환을 위한 룩업테이블 생성
	butterflyLookupTable.assign(size * passes * 4, float());

	for (int i = 0; i < passes; i++)
	{
		int nBlocks = (int)pow(2, passes - 1 - i);
		int nHInputs = (int)pow(2, i);

		for (int j = 0; j < nBlocks; j++)
		{
			for (int k = 0; k < nHInputs; k++)
			{
				int i1, i2, j1, j2;
				if (i == 0)
				{
					i1 = j * nHInputs * 2 + k;
					i2 = j * nHInputs * 2 + nHInputs + k;
					j1 = BitReverse(i1);
					j2 = BitReverse(i2);
				}
				else
				{
					i1 = j * nHInputs * 2 + k;
					i2 = j * nHInputs * 2 + nHInputs + k;
					j1 = i1;
					j2 = i2;
				}

				float wr = cosf(2.0f * D3DX_PI * (float)(k * nBlocks) / fSize);
				float wi = sinf(2.0f * D3DX_PI * (float)(k * nBlocks) / fSize);

				int offset1 = 4 * (i1 + i * size);
				butterflyLookupTable[offset1 + 0] = j1;
				butterflyLookupTable[offset1 + 1] = j2;
				butterflyLookupTable[offset1 + 2] = wr;
				butterflyLookupTable[offset1 + 3] = wi;

				int offset2 = 4 * (i2 + i * size);
				butterflyLookupTable[offset2 + 0] = j1;
				butterflyLookupTable[offset2 + 1] = j2;
				butterflyLookupTable[offset2 + 2] = -wr;
				butterflyLookupTable[offset2 + 3] = -wi;

			}
		}
	}

	//분산 테이블 구성
	UINT nPlus1 = vertexLength + 1;
	dispersionTable.assign(nPlus1 * nPlus1, float());
	for (int m_prime = 0; m_prime < nPlus1; m_prime++)
	{
		for (int n_prime = 0; n_prime < nPlus1; n_prime++)
		{
			int index = m_prime * nPlus1 + n_prime;
			dispersionTable[index] = this->Dispersion(n_prime, m_prime);
		}
	}

	spectrum.assign(nPlus1 * nPlus1, D3DXVECTOR2());
	spectrum_conj.assign(nPlus1 * nPlus1, D3DXVECTOR2());
	//스펙트럼 및 정점들의 초기화 
	for (int m_prime = 0; m_prime < nPlus1; m_prime++)
	{
		for (int n_prime = 0; n_prime < nPlus1; n_prime++)
		{
			int index = m_prime * nPlus1 + n_prime;

			VertexTextureNormal vertex;

			spectrum[index] = this->GetSpectrum(n_prime, m_prime);

			spectrum_conj[index] = this->GetSpectrum(-n_prime, -m_prime);
			spectrum_conj[index].y *= -1.0f;

			vertex.position.x = n_prime * length / vertexLength;
			vertex.position.y = 0.0f;
			vertex.position.z = m_prime * length / vertexLength;

			vertex.normal = D3DXVECTOR3(0.f, 1.f, 0.f);

			vertex.uv.x = 1.0f / (float)nPlus1;
			vertex.uv.y = 1.0f / (float)nPlus1;

			this->vertexData.push_back(vertex);
			positionData.push_back(Vertex(vertex.position));
		}
	}

	UINT index = 0;
	for (UINT z = 0; z < nPlus1 - 1; z++)
	{
		for (UINT x = 0; x < nPlus1 - 1; x++)
		{
			indexData.push_back((nPlus1)* z + x);				//0
			indexData.push_back((nPlus1) * (z + 1) + x);		//1
			indexData.push_back((nPlus1)* z + x + 1);			//2

			indexData.push_back((nPlus1)* z + x + 1);			//0
			indexData.push_back((nPlus1) * (z + 1) + x);		//1
			indexData.push_back((nPlus1) * (z + 1) + x + 1);	//2

			index += 6;
		}
	}
	//인스턴스 그리드 설정
	for (UINT z = 0; z < gridCountZ; ++z)
	{
		for (UINT x = 0; x < gridCountX; ++x)
		{
			D3DXVECTOR2 offset;
			offset.x = length * x;
			offset.y = length * z;
			instanceData.push_back(offset);
		}
	}

	for (UINT y = 0; y < 2; ++y)
	{
		for (UINT x = 0; x < vertexLength * vertexLength; ++x)
		{
			heightData.push_back(StructuredDataFloat2(D3DXVECTOR2(0.f, 0.f), x, y));
			slopeData.push_back(StructuredDataFloat4(D3DXVECTOR4(0.f, 0.f, 0.f, 0.f), x, y));
			displacementData.push_back(StructuredDataFloat4(D3DXVECTOR4(0.f, 0.f, 0.f, 0.f), x, y));
		}
	}

	Buffer::CreateDynamicVertexBuffer(&vertexBuffer, vertexData.data(), sizeof VertexTextureNormal * vertexData.size());
	Buffer::CreateDynamicVertexBuffer(&instanceBuffer, instanceData.data(), sizeof D3DXVECTOR2 * instanceData.size());
	Buffer::CreateIndexBuffer(&indexBuffer, indexData.data(), indexData.size());

	computeSpectrumShader = new ComputeShader(ShaderPath + L"005_OceanCompute.hlsl", "CSMain0");
	peformFFTShader0 = new ComputeShader(ShaderPath + L"005_OceanCompute.hlsl", "CSMain1");
	peformFFTShader1 = new ComputeShader(ShaderPath + L"005_OceanCompute.hlsl", "CSMain2");
	updateVertexShader = new ComputeShader(ShaderPath + L"005_OceanCompute.hlsl", "CSMain3");
	oceanComputeBuffer = new OceanComputeBuffer;
	oceanComputeBuffer->data.vertexLength = vertexLength;
	oceanComputeBuffer->data.length = length;
	oceanComputeBuffer->data.time = 0.0f;

	rwHeightBuffer = new CResource1D(sizeof StructuredDataFloat2, heightData.size(), heightData.data());
	rwSlopeBuffer = new CResource1D(sizeof StructuredDataFloat4, slopeData.size(), slopeData.data());
	rwDisplacementBuffer = new CResource1D(sizeof StructuredDataFloat4, displacementData.size(), displacementData.data());

	spectrumBuffer = new CResource1D(sizeof D3DXVECTOR2, spectrum.size(), spectrum.data());
	spectrum_conjBuffer = new CResource1D(sizeof D3DXVECTOR2, spectrum_conj.size(), spectrum_conj.data());
	dispersionTableBuffer = new CResource1D(sizeof(float), dispersionTable.size(), dispersionTable.data());
	butterflyLookupTableBuffer = new CResource1D(sizeof(float), butterflyLookupTable.size(), butterflyLookupTable.data());

	originVertexBuffer = new CResource1D(sizeof D3DXVECTOR3, positionData.size(), positionData.data());
	vertexDataBuffer = new CResource1D(sizeof VertexTextureNormal, vertexData.size(), vertexData.data());

	//정점변환을 위한 데이터들은 전부 버퍼로 바인딩 했으므로 메모리 해제.
	heightData.clear();
	slopeData.clear();
	displacementData.clear();

	spectrum.clear();
	spectrum_conj.clear();
	dispersionTable.clear();
	butterflyLookupTable.clear();
	positionData.clear();
}

