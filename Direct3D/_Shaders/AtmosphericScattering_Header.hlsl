#define PI 3.14159265359

cbuffer ViewProjectionBuffer : register(b0)
{
    matrix View;
    matrix Projection;
    matrix ViewProjection;
    matrix InvView;
    matrix InvProjection;
    matrix InvViewProjection;
}
cbuffer WorldBuffer : register(b1)
{
    matrix World;
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

    float4 _FrustumCorners[4];

    float4 _LightDir;

    float _SunIntensity;
    float3 _inscatteringLUTSize;

    float4 _testAmbient;

}



struct G_Buffer
{
    float4 normal : SV_Target0;
    float4 diffuse : SV_Target1;
    float4 spec : SV_Target2;
    float4 worldPos : SV_Target3;
};
G_Buffer PackGBuffer(G_Buffer buffer, float3 normal, float3 diffuse, float SpecIntensity, float SpecPower)
{
    G_Buffer Out = buffer;

	// Normalize the specular power
    float SpecPowerNorm = 0.0f; //max(0.0001, (SpecPower - g_SpecPowerRange.x) / g_SpecPowerRange.y);

	// Pack all the data into the GBuffer structure
    Out.diffuse = float4(diffuse.rgb, SpecIntensity);
    Out.normal = float4(normal * 0.5f + 0.5f, 1.0f);
    Out.spec = float4(SpecPowerNorm, 0.0f, 0.0f, 1.0f);

    return Out;

}



RWTexture2D<float4> _ParticleDensityLUTCompute : register(u0);
Texture2D<float4> _ParticleDensityLUT : register(t0);

Texture2D _RandomVectors;

Texture3D _SkyboxLUT : register(t1);

Texture3D _InscatteringLUT;
Texture3D _ExtinctionLUT;

SamplerState _linear;

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
// Sun
//-----------------------------------------------------------------------------------------
float Sun(float cosAngle)
{
    float g = 0.98;
    float g2 = g * g;

    float sun = pow(1 - g, 2.0) / (4 * PI * pow(1.0 + g2 - 2.0 * g * cosAngle, 1.5));
    return sun * 0.003; // 5;
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
// ApplyPhaseFunctionElek
//-----------------------------------------------------------------------------------------
void ApplyPhaseFunctionElek(inout float3 scatterR, inout float3 scatterM, float cosAngle)
{
	// r
    float phase = (8.0 / 10.0) / (4 * PI) * ((7.0 / 5.0) + 0.5 * cosAngle);
    scatterR *= phase;

	// m
    float g = _MieG;
    float g2 = g * g;
    phase = (1.0 / (4.0 * PI)) * ((3.0 * (1.0 - g2)) / (2.0 * (2.0 + g2))) * ((1 + cosAngle * cosAngle) / (pow((1 + g2 - 2 * g * cosAngle), 3.0 / 2.0)));
    scatterM *= phase;
}

//-----------------------------------------------------------------------------------------
// RenderSun
//-----------------------------------------------------------------------------------------
float3 RenderSun(in float3 scatterM, float cosAngle)
{
    return scatterM * Sun(cosAngle);
}

//-----------------------------------------------------------------------------------------
// GetAtmosphereDensity
//-----------------------------------------------------------------------------------------
void GetAtmosphereDensity(float3 position, float3 planetCenter, float3 lightDir, out float2 localDensity, out float2 densityToAtmTop)
{
    float height = length(position - planetCenter) - _PlanetRadius;
    localDensity = exp(-height.xx / _DensityScaleHeight.xy);

    float cosAngle = dot(normalize(position - planetCenter), -lightDir.xyz);

    densityToAtmTop = _ParticleDensityLUT.Sample(_linear, float2(cosAngle * 0.5 + 0.5, (height / _AtmosphereHeight))).rg;
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
// ComputeOpticalDepth
//-----------------------------------------------------------------------------------------
float3 ComputeOpticalDepth(float2 density)
{
    float3 Tr = density.x * _ExtinctionR;
    float3 Tm = density.y * _ExtinctionM;

    float3 extinction = exp(-(Tr + Tm));

    return _IncomingLight.xyz * extinction;
}

//-----------------------------------------------------------------------------------------
// IntegrateInscattering
//-----------------------------------------------------------------------------------------
float4 IntegrateInscattering(float3 rayStart, float3 rayDir, float rayLength, float3 planetCenter, float distanceScale, float3 lightDir, float sampleCount, out float4 extinction)
{
    float3 step = rayDir * (rayLength / sampleCount);
    float stepSize = length(step) * distanceScale;

    float2 densityCP = 0;
    float3 scatterR = 0;
    float3 scatterM = 0;

    float2 localDensity;
    float2 densityPA;

    float2 prevLocalDensity;
    float3 prevLocalInscatterR, prevLocalInscatterM;
    GetAtmosphereDensity(rayStart, planetCenter, lightDir, prevLocalDensity, densityPA);
    ComputeLocalInscattering(prevLocalDensity, densityPA, densityCP, prevLocalInscatterR, prevLocalInscatterM);

    float2 densityCPA = densityCP + densityPA;
    float3 Tr = densityCPA.x * _ExtinctionR;
    float3 Tm = densityCPA.y * _ExtinctionM;
    float3 extinctiontemp = exp(-(Tr + Tm));


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

    float3 m = scatterM;
	// phase function
    ApplyPhaseFunction(scatterR, scatterM, dot(rayDir, -lightDir.xyz));
	//scatterR = 0;
    float3 lightInscatter = (scatterR * _ScatteringR + scatterM * _ScatteringM) * _IncomingLight.xyz;

    lightInscatter += RenderSun(m, dot(rayDir, -lightDir.xyz)) * _SunIntensity;
    float3 lightExtinction = exp(-(densityCP.x * _ExtinctionR + densityCP.y * _ExtinctionM));

    extinction = float4(lightExtinction, 0);


    return float4(lightInscatter, 1);
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
// PrecomputeAmbientLight
//-----------------------------------------------------------------------------------------
float4 PrecomputeAmbientLight(float3 lightDir)
{
    float startHeight = 0;
    float3 rayStart = float3(0, startHeight, 0);
    float3 planetCenter = float3(0, -_PlanetRadius + startHeight, 0);

    float4 color = 0;

    int sampleCount = 255;

	[loop]
    for (int ii = 0; ii < sampleCount; ++ii)
    {
        float3 rayDir = float3(1, 0, 0); //_RandomVectors.Sample(_linear, float2(ii + (0.5 / 255.0), 0.5));
        rayDir.y = abs(rayDir.y);

        float2 intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius + _AtmosphereHeight);
        float rayLength = intersection.y;

        intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius);
        if (intersection.x > 0)
            rayLength = min(rayLength, intersection.x);

        float sampleCount = 32;
        float4 extinction;

        float4 scattaring = (IntegrateInscattering(rayStart, rayDir, rayLength, planetCenter, 1, lightDir, sampleCount, extinction));

        color += scattaring * dot(rayDir, float3(0, 1, 0));
    }

    return color * 2 * PI / sampleCount;
}

//-----------------------------------------------------------------------------------------
// PrecomputeDirLight
//-----------------------------------------------------------------------------------------
float4 PrecomputeDirLight(float3 rayDir)
{
    float startHeight = 500;

    float3 rayStart = float3(0, startHeight, 0);

    float3 planetCenter = float3(0, -_PlanetRadius + startHeight, 0);

    float2 localDensity;
    float2 densityToAtmosphereTop;

    GetAtmosphereDensity(rayStart, planetCenter, -rayDir, localDensity, densityToAtmosphereTop);
    float4 color;
    color.xyz = ComputeOpticalDepth(densityToAtmosphereTop);
    color.w = 1;
    return color;
}