
RWTexture2D<float4> _ParticleDensityLUTCompute : register(u0);
Texture2D<float4> _ParticleDensityLUT : register(t0);

RWTexture3D<float4> _SkyboxLUT : register(u1);

SamplerState sampler_ParticleDensityLUT;
SamplerState PointClampSampler;
SamplerState LinearClampSampler;



cbuffer ViewProjectionBuffer : register(b0)
{
    matrix View;
    matrix Projection;
    matrix ViewProjection;
    matrix InvView;
    matrix InvProjection;
    matrix InvViewProjection;
}

cbuffer Buffers : register(b2)
{
    float _AtmosphereHeight; //AtmosphereHeight
    float _PlanetRadius; //PlanetRadius
    float2 _DensityScaleHeight; //DensityScale

    float3 _ScatteringR; //RayleighSct * RayleighScatterCoef
    float Padding0;
    float3 _ScatteringM; //MieSct * MieScatterCoef
    float Padding1;
    float3 _ExtinctionR; //RayleighSct * RayleighExtinctionCoef
    float Padding2;
    float3 _ExtinctionM; //MieSct * MieExtinctionCoef
    float Padding3;

    float4 _IncomingLight; //IncomingLight

    float _MieG; //MieG
    float _DistanceScale; //DistanceScale
    float2 Padding4;

    float4 _SunColor;


    float4 _LightDir;

    float _SunIntensity;
    float3 _inscatteringLUTSize;

    float4 _testAmbient;

}


float4 _BottomLeftCorner;
float4 _BottomRightCorner;
float4 _TopLeftCorner;
float4 _TopRightCorner;

float4 _CameraPos;

#define PI 3.14159265359

//-----------------------------------------------------------------------------------------
// ScatteringOutput
//-----------------------------------------------------------------------------------------
struct ScatteringOutput
{
    float3 rayleigh;
    float3 mie;
};

//-----------------------------------------------------------------------------------------
// RaySphereIntersection
//-----------------------------------------------------------------------------------------
float2 RaySphereIntersection(float3 rayOrigin, float3 rayDir, float3 sphereCenter, float sphereRadius)
{
    rayOrigin -= sphereCenter;
    float a = dot(rayDir, rayDir);
    float b = 2.0 * dot(rayOrigin, rayDir);
    float c = dot(rayOrigin, rayOrigin) - (sphereRadius * sphereRadius);
    float d = b * b - 4 * a * c;
    if (d < 0)
    {
        return -1;
    }
    else
    {
        d = sqrt(d);
        return float2(-b - d, -b + d) / (2 * a);
    }
}

//-----------------------------------------------------------------------------------------
// GetAtmosphereDensity
//-----------------------------------------------------------------------------------------
void GetAtmosphereDensity(float3 position, float3 planetCenter, float3 lightDir, out float2 localDensity, out float2 densityToAtmTop)
{
    float height = length(position - planetCenter) - _PlanetRadius;
    localDensity = exp(-height.xx / _DensityScaleHeight.xy);

    float cosAngle = dot(normalize(position - planetCenter), -lightDir.xyz);

	//densityToAtmTop = _ParticleDensityLUT.SampleLevel(PointClampSampler, float2(cosAngle * 0.5 + 0.5, (height / _AtmosphereHeight)), 1.0).xy;
    densityToAtmTop = _ParticleDensityLUT.SampleLevel(LinearClampSampler, float2(cosAngle * 0.5 + 0.5, (height / _AtmosphereHeight)), 0.0).xy;
}

//-----------------------------------------------------------------------------------------
// ComputeLocalInscattering
//-----------------------------------------------------------------------------------------
void ComputeLocalInscattering(float2 localDensity, float2 densityPA, float2 densityCP, out float3 localInscatterR, out float3 localInscatterM)
{
    float2 densityCPA = densityCP + densityPA;

    float3 Tr = densityCPA.x * _ExtinctionR;
    float3 Tm = densityCPA.y * _ExtinctionM;

    float3 extinction = exp(-(Tr + Tm));

    localInscatterR = localDensity.x * extinction;
    localInscatterM = localDensity.y * extinction;
}

//-----------------------------------------------------------------------------------------
// ApplyPhaseFunction
//-----------------------------------------------------------------------------------------
void ApplyPhaseFunction(inout float3 scatterR, inout float3 scatterM, float cosAngle)
{
	// r
    float phase = (3.0 / (16.0 * PI)) * (1 + (cosAngle * cosAngle));
    scatterR *= phase;

	// m
    float g = _MieG;
    float g2 = g * g;
    phase = (1.0 / (4.0 * PI)) * ((3.0 * (1.0 - g2)) / (2.0 * (2.0 + g2))) * ((1 + cosAngle * cosAngle) / (pow((1 + g2 - 2 * g * cosAngle), 3.0 / 2.0)));
    scatterM *= phase;
}

//-----------------------------------------------------------------------------------------
// IntegrateInscattering
//-----------------------------------------------------------------------------------------
ScatteringOutput IntegrateInscattering(float3 rayStart, float3 rayDir, float rayLength, float3 planetCenter, float3 lightDir)
{
    float sampleCount = 64;
    float3 step = rayDir * (rayLength / sampleCount);
    float stepSize = length(step);

    float2 densityCP = 0;
    float3 scatterR = 0;
    float3 scatterM = 0;

    float2 localDensity;
    float2 densityPA;

    float2 prevLocalDensity;
    float3 prevLocalInscatterR, prevLocalInscatterM;
    GetAtmosphereDensity(rayStart, planetCenter, lightDir, prevLocalDensity, densityPA);
    ComputeLocalInscattering(prevLocalDensity, densityPA, densityCP, prevLocalInscatterR, prevLocalInscatterM);

	// P - current integration point
	// C - camera position
	// A - top of the atmosphere
	[loop]
    for (float s = 1.0; s < sampleCount; s += 1)
    {
        float3 p = rayStart + step * s;

        GetAtmosphereDensity(p, planetCenter, lightDir, localDensity, densityPA);
        densityCP += (localDensity + prevLocalDensity) * (stepSize / 2.0);

        prevLocalDensity = localDensity;

        float3 localInscatterR, localInscatterM;
        ComputeLocalInscattering(localDensity, densityPA, densityCP, localInscatterR, localInscatterM);
		
        scatterR += (localInscatterR + prevLocalInscatterR) * (stepSize / 2.0);
        scatterM += (localInscatterM + prevLocalInscatterM) * (stepSize / 2.0);

        prevLocalInscatterR = localInscatterR;
        prevLocalInscatterM = localInscatterM;
    }

    ScatteringOutput output;
    output.rayleigh = scatterR;
    output.mie = scatterM;

    return output;
}

//-----------------------------------------------------------------------------------------
// PrecomputeParticleDensity
//-----------------------------------------------------------------------------------------
float2 PrecomputeParticleDensity(float3 rayStart, float3 rayDir)
{
    float3 planetCenter = float3(0, -_PlanetRadius, 0);

    float stepCount = 250;

    float2 intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius);
    if (intersection.x > 0)
    {
		// intersection with planet, write high density
        return 1e+20;
    }

    intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius + _AtmosphereHeight);
    float3 rayEnd = rayStart + rayDir * intersection.y;

	// compute density along the ray
    float3 step = (rayEnd - rayStart) / stepCount;
    float stepSize = length(step);
    float2 density = 0;

    for (float s = 0.5; s < stepCount; s += 1.0)
    {
        float3 position = rayStart + step * s;
        float height = abs(length(position - planetCenter) - _PlanetRadius);
        float2 localDensity = exp(-(height.xx / _DensityScaleHeight));

        density += localDensity * stepSize;
    }

    return density;
}

//-----------------------------------------------------------------------------------------
// SkyboxLUT
//-----------------------------------------------------------------------------------------
[numthreads(1, 1, 1)]
void SkyboxLUT(uint3 id : SV_DispatchThreadID)
{
    float w, h, d;
    _SkyboxLUT.GetDimensions(w, h, d);

	// linear parameters
    float3 coords = float3(id.x / (w - 1), id.y / (h - 1), id.z / (d - 1));

    float height = coords.x * coords.x * _AtmosphereHeight;
	//float height = coords.x * _AtmosphereHeight;
    float ch = -(sqrt(height * (2 * _PlanetRadius + height)) / (_PlanetRadius + height));

    float viewZenithAngle = coords.y;
	//float viewZenithAngle = coords.y * 2.0 - 1.0;

    if (viewZenithAngle > 0.5)
    {
        viewZenithAngle = ch + pow((viewZenithAngle - 0.5) * 2, 5) * (1 - ch);
    }
    else
    {
        viewZenithAngle = ch - pow(viewZenithAngle * 2, 5) * (1 + ch);
    }

    float sunZenithAngle = (tan((2 * coords.z - 1 + 0.26) * 0.75)) / (tan(1.26 * 0.75)); // coords.z * 2.0 - 1.0;
	//float sunZenithAngle = coords.z * 2.0 - 1.0;

    float3 planetCenter = float3(0, -_PlanetRadius, 0);
    float3 rayStart = float3(0, height, 0);

    float3 rayDir = float3(sqrt(saturate(1 - viewZenithAngle * viewZenithAngle)), viewZenithAngle, 0);
    float3 lightDir = -float3(sqrt(saturate(1 - sunZenithAngle * sunZenithAngle)), sunZenithAngle, 0);

    float rayLength = 1e20;
    float2 intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius + _AtmosphereHeight);
    rayLength = intersection.y;

    intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius);
    if (intersection.x > 0)
        rayLength = min(rayLength, intersection.x);

    ScatteringOutput scattering = IntegrateInscattering(rayStart, rayDir, rayLength, planetCenter, lightDir);
	
	//color.inscattering.z = coords.z;
    _SkyboxLUT[id.xyz] = float4(scattering.rayleigh.xyz, scattering.mie.x);

}
//-----------------------------------------------------------------------------------------
// particleDensityLUT
//-----------------------------------------------------------------------------------------

[numthreads(16, 16, 1)]
void particleDensityLUT(uint3 id : SV_DispatchThreadID)
{
    //_ParticleDensityLUTCompute
    //720 * 720
    float2 uv = id.xy / 720.f;
    float cosAngle = uv.x * 2.0 - 1.0;
    float sinAngle = sqrt(saturate(1 - cosAngle * cosAngle));
    float startHeight = lerp(0.0, _AtmosphereHeight, uv.y);
    
    float3 rayStart = float3(0, startHeight, 0);
    float3 rayDir = float3(sinAngle, cosAngle, 0);
              
    _ParticleDensityLUTCompute[id.xy] = float4(PrecomputeParticleDensity(rayStart, rayDir), 0.0f, 1.0f);

}