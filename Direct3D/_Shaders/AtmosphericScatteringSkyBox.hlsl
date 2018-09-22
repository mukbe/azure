#include "AtmosphericScattering_Header.hlsl"

			
struct VertexINPUT
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;

};

struct PSINPUT
{
    float4 pos : SV_POSITION;
    float3 vertex : TEXCOORD0;
};

PSINPUT VS(VertexINPUT input)
{
    PSINPUT o;
    float4 v = input.position;
    o.pos = mul(v, World);
    o.pos = mul(o.pos, ViewProjection);
    o.vertex = v.xyz;
   
    return o;
}
			
G_Buffer PS(PSINPUT i)
{
    G_Buffer output;

    float3 rayStart = InvView[3].xyz;
    float3 rayDir = normalize(mul((float3x3) World, i.vertex));

    float3 lightDir = _LightDir.xyz;

    float3 planetCenter = InvView[3].xyz;
    planetCenter = float3(0, -_PlanetRadius, 0);

    float2 intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius + _AtmosphereHeight);
    float rayLength = intersection.y;

    intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius);
    if (intersection.x > 0)
        rayLength = min(rayLength, intersection.x);

    float4 extinction;
    float4 inscattering = IntegrateInscattering(rayStart, rayDir, rayLength, planetCenter, 1, lightDir, 16, extinction);
    
    //º¸Á¤°ª
    float4 color = _AddAmbient;
    
    color *= 0.3f;
    output.diffuse = float4(inscattering.xyz + color.xyz, 1);
    return output;
}

PSINPUT UseSkyBoxVS(VertexINPUT input)
{
    PSINPUT o;
    float4 v = input.position;
    o.pos = mul(v, World);
    o.pos = mul(o.pos, ViewProjection);
    o.vertex = v.xyz;
    return o;
}

G_Buffer UseSkyBoxPS(PSINPUT i)
{
    G_Buffer output;

    float3 rayStart = InvView[3].xyz;
    float3 rayDir = normalize(mul((float3x3) World, i.vertex));

    float3 lightDir = _LightDir.xyz;

    float3 planetCenter = InvView[3].xyz;
    planetCenter = float3(0, -_PlanetRadius, 0);

    float4 scatterR = 0;
    float4 scatterM = 0;

    float height = length(rayStart - planetCenter) - _PlanetRadius;
    float3 normal = normalize(rayStart - planetCenter);

    float viewZenith = dot(normal, rayDir);
    float sunZenith = dot(normal, -lightDir);

    float3 coords = float3(height / _AtmosphereHeight, viewZenith * 0.5 + 0.5 + 0.1f, sunZenith * 0.5 + 0.5);

    coords.x = pow(height / _AtmosphereHeight, 0.5);
    float ch = -(sqrt(height * (2 * _PlanetRadius + height)) / (_PlanetRadius + height));
    if (viewZenith > ch)
    {
        coords.y = 0.5f * pow((viewZenith - ch) / (1 - ch), 0.2) + 0.5f;
    }
    else
    {
        coords.y = 0.5f * pow((ch - viewZenith) / (ch + 1), 0.2);
    }
    coords.z = 0.5f * ((atan(max(sunZenith, -0.1975f) * tan(1.26 * 1.1)) / 1.1) + (1 - 0.26));

    scatterR = _SkyboxLUT.SampleLevel(_linear, coords, 0.0);
    scatterM.xyz = scatterR.xyz * ((scatterR.w) / (scatterR.x)); // *(_ScatteringR.x / _ScatteringM.x) * (_ScatteringM / _ScatteringR);

    float3 m = scatterM.xyz;

    ApplyPhaseFunctionElek(scatterR.xyz, scatterM.xyz, dot(rayDir, -lightDir.xyz));
    float3 lightInscatter = (scatterR.xyz * _ScatteringR + scatterM.xyz * _ScatteringM) * _IncomingLight.xyz;


    lightInscatter += RenderSun(m, dot(rayDir, -lightDir.xyz)) * _SunIntensity;
    float4 last = float4(max(0, lightInscatter), 1);

    float4 color = _AddAmbient;
    color *= 0.2f;
    output.diffuse = float4(last.xyz + color.xyz, 1);
    output.normal.a = 1.5f;
    return output;
}
