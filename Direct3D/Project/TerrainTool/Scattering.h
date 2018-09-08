#pragma once
#include "./Utilities/GeometryGenerator.h"

namespace Environment
{
	class Sun;
}

class Scattering
{
private:
	enum RenderMode
	{
		Reference,
		Optimized
	};
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
		}

		struct Struct
		{
			float _AtmosphereHeight;			//AtmosphereHeight
			float _PlanetRadius;				//PlanetRadius
			D3DXVECTOR2 _DensityScaleHeight;	//DensityScale

			D3DXVECTOR3 _ScatteringR;			//RayleighSct * RayleighScatterCoef
			float Padding0;
			D3DXVECTOR3 _ScatteringM;			//MieSct * MieScatterCoef
			float Padding1;
			D3DXVECTOR3 _ExtinctionR;			//RayleighSct * RayleighExtinctionCoef
			float Padding2;
			D3DXVECTOR3 _ExtinctionM;			//MieSct * MieExtinctionCoef
			float Padding3;

			D3DXCOLOR _IncomingLight;			//IncomingLight

			float _MieG;						//MieG
			float _DistanceScale;				//DistanceScale
			D3DXVECTOR2 Padding4;

			D3DXCOLOR _SunColor;

			D3DXPLANE _FrustumCorners[4];

			D3DXVECTOR4 _LightDir;

			float _SunIntensity;
			D3DXVECTOR3 _inscatteringLUTSize;
			
			D3DXCOLOR testAmbient;
		}Data;
	};
public:
	Scattering(class FreeCamera * camera);
	~Scattering();

	void Updata();
	void Render();
	void UIRender();

private:
	void UpdateMaterialParameters();
	void PrecomputeParticleDensity();
	void CalculateLightLUTs();
	void InitializeInscatteringLUT();
	void PrecomputeSkyboxLUT();

	void UpdateInscatteringLUT();

	D3DXCOLOR ComputeLightColor();
	void UpdateDirectionalLightColor(D3DXCOLOR sunColor);

	void UpdateAmbientLightColor(D3DXCOLOR ambient);
	D3DXCOLOR ComputeAmbientColor();

private:
	RenderMode RenderingMode = RenderMode::Optimized;
	
	//�ٸ� ��ǻƮ ���̴��� �ʿ��� �� �� ���� LUT�� ����Ƽ�� Shader PASS�� ������ 
	//���⼱ ��ǻ���� �ϴ°� �� ����� ����
	ComputeShader* ScatteringComputeShader;
	ComputeShader* precomputeSkyboxLUT;

	ComputeShader* particleDensityLUTComputeShader;

	Shader* shader;
	WorldBuffer * world;

	Environment::Sun* sun;
	CResource2D* _particleDensityLUT = nullptr;
	CResource1D* _lightColorTexture;
	CResource1D* _lightColorTextureTemp;

	D3DXVECTOR3 _skyboxLUTSize = D3DXVECTOR3(32, 128, 32);

	CResource3D* _skyboxLUT;

	D3DXVECTOR3 _inscatteringLUTSize = D3DXVECTOR3(8, 8, 64);
	CResource3D* _inscatteringLUT;
	CResource3D* _extinctionLUT;

	const int LightLUTSize = 128;

	//0~15
	vector<D3DXCOLOR> _directionalLightLUT;
	//0~10
	vector<D3DXCOLOR> _ambientLightLUT;

	//Material _material;
	Buffer* buffer;
	FreeCamera* _camera;

	D3DXCOLOR _sunColor;

	//1~64
	int SampleCount = 16;
	float MaxRayLength = 400;

	//0~10
	D3DXCOLOR IncomingLight = D3DXCOLOR(4,4,4,4);


	//0~10
	float RayleighScatterCoef = 1;
	//0~10
	float RayleighExtinctionCoef = 1;
	//0~10
	float MieScatterCoef = 1;
	//0~10
	float MieExtinctionCoef = 1;
	//0~0.999f
	float MieG = 0.76f;
	float DistanceScale = 1;


	bool UpdateLightColor = true;

	//0.5~3.0
	float LightColorIntensity = 1.0f;
	bool UpdateAmbientColor = true;
	//0.5~3.0
	float AmbientColorIntensity = 1.0f;
	float SunIntensity = 1;

	const float AtmosphereHeight = 80000.0f;
	const float PlanetRadius = 6371000.0f;
	const D3DXVECTOR2 DensityScale = D3DXVECTOR2(7994.0f, 1200.0f);
	const D3DXVECTOR3 RayleighSct = D3DXVECTOR3(5.8f, 13.5f, 33.1f) * 0.000001f;
	const D3DXVECTOR3 MieSct = D3DXVECTOR3(2.0f, 2.0f, 2.0f) * 0.00001f;

	D3DXVECTOR3 _FrustumCorners[4];


	ID3D11Buffer* vertexBuffer, *indexBuffer;
	struct GeometryGenerator::MeshData meshData;

	void CreateBuffer();
};
