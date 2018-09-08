#include "AtmosphericScattering_Header.hlsl"

struct v2f
{
    float4 pos : SV_POSITION;
    float3 vertex : TEXCOORD0;
};
			
struct VertexTexture
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
};

v2f VS(VertexTexture input)
{
    v2f o;
    float4 v = input.position;
    o.pos = mul(v, World);
    o.pos = mul(o.pos, ViewProjection);
    o.vertex = v.xyz;
    return o;
}
			
G_Buffer PS(v2f i)
{
    G_Buffer output;

    float3 rayStart = InvView[3].xyz;
    float3 rayDir = normalize(mul((float3x3) World, i.vertex));

    float3 lightDir = _LightDir.xyz;

    float3 planetCenter = InvView[3].xyz;
    planetCenter = float3(0, -_PlanetRadius, 0);

    //==============================================
    float2 intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius + _AtmosphereHeight);
    float rayLength = intersection.y;

    intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius);
    if (intersection.x > 0)
        rayLength = min(rayLength, intersection.x);


    float4 extinction, testColor;
    float4 inscattering = IntegrateInscattering(testColor ,rayStart, rayDir, rayLength, planetCenter, 1, lightDir, 16, extinction);

    output.diffuse = float4(inscattering.xyz, 1);

    return output;
    //===============일단 아래에 있는건 무시

    float4 scatterR = 0;
    float4 scatterM = 0;

    float height = length(rayStart - planetCenter) - _PlanetRadius;
    float3 normal = normalize(rayStart - planetCenter);

    float viewZenith = dot(normal, rayDir);
    float sunZenith = dot(normal, -lightDir);

    float3 coords = float3(height / _AtmosphereHeight, viewZenith * 0.5 + 0.5, sunZenith * 0.5 + 0.5);

    coords.x = pow(height / _AtmosphereHeight, 0.5);
    float ch = -(sqrt(height * (2 * _PlanetRadius + height)) / (_PlanetRadius + height));
    if (viewZenith > ch)
    {
        coords.y = 0.5 * pow((viewZenith - ch) / (1 - ch), 0.2) + 0.5;
    }
    else
    {
        coords.y = 0.5 * pow((ch - viewZenith) / (ch + 1), 0.2);
    }
    coords.z = 0.5 * ((atan(max(sunZenith, -0.1975) * tan(1.26 * 1.1)) / 1.1) + (1 - 0.26));
    
    scatterR = _SkyboxLUT.Sample(_linear, coords);
    scatterM.xyz = scatterR.xyz * ((scatterR.w) / (scatterR.x)); // *(_ScatteringR.x / _ScatteringM.x) * (_ScatteringM / _ScatteringR);

    float3 m = scatterM.xyz;
				//scatterR = 0;
				// phase function
    ApplyPhaseFunctionElek(scatterR.xyz, scatterM.xyz, dot(rayDir, -lightDir.xyz));
    float3 lightInscatter = (scatterR.xyz * _ScatteringR + scatterM.xyz * _ScatteringM) * _IncomingLight.xyz;

	//float3 m = float3(0, 0, 0);
	//float3 lightInscatter = float3(0, 0, 0);
    //return float4(0.5f, 0.5f, 0.5f, 0.1f);

    lightInscatter += RenderSun(m, dot(rayDir, -lightDir.xyz)) * _SunIntensity;
	//return float4(max(0, lightInscatter), 1);




}
