#pragma once
#include "./Renders/ShaderBuffer.h"
#include "./Object/GameObject/GameObject.h"

class Ocean : public GameObject
{
private:
	class InstanceShader*			instanceShader;
	class Material*					material;
	class WorldBuffer*				worldBuffer;
	ID3D11Buffer *					vertexBuffer;
	ID3D11Buffer*					instanceBuffer;
	ID3D11Buffer*					indexBuffer;

	vector<D3DXVECTOR2>				instanceData;
	vector<VertexTextureNormal>		vertexData;
	vector<UINT>					indexData;
private:
	class ComputeShader*			computeSpectrumShader;
	class ComputeShader*			peformFFTShader0;
	class ComputeShader*			peformFFTShader1;
	class ComputeShader*			updateVertexShader;
	class OceanComputeBuffer*		oceanComputeBuffer;

	class CResource1D*				rwHeightBuffer;
	class CResource1D*				rwSlopeBuffer;
	class CResource1D*				rwDisplacementBuffer;

	class CResource1D*				spectrumBuffer;
	class CResource1D*				spectrum_conjBuffer;
	class CResource1D*				dispersionTableBuffer;
	class CResource1D*				butterflyLookupTableBuffer;

	class CResource1D*				originVertexBuffer;
	class CResource1D*				vertexDataBuffer;

	class Texture*					fresnelLookUp;
private:
	D3DXCOLOR						oceanColor;
	UINT							gridCountX;
	UINT							gridCountZ;
	UINT							vertexLength;
	int								passes;
	float							length;
	float							waveAmp;
	D3DXVECTOR2						windSpeed;
	D3DXVECTOR2						windDirection;
public:
	Ocean();
	 ~Ocean();

	virtual void Update()override;
	virtual void Render()override;
	virtual void UIRender()override;
private:
	void InitInstanceShader();
	void InitOceansData();
	void InitBuffers();
	void CreateFresnelLookUpTable();
	void UpdateBuffer();
	void ComputingOcean();
private:
	D3DXVECTOR2 GetSpectrum(int n_prime, int m_prime);
	D3DXVECTOR2 GaussianRandomVariable();
	float Dispersion(int n_prime, int m_prime);
	float PhillipsSpectrum(int n_prime, int m_prime);
	int BitReverse(int i);
};


class OceanComputeBuffer : public ShaderBuffer
{
public:
	struct Struct {
		UINT vertexLength;
		float length;
		float time;
		int nowPass;
	}data;
public:
	OceanComputeBuffer()
		:ShaderBuffer(&data, sizeof Struct) {}
};