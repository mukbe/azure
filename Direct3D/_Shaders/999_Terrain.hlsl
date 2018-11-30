#include "000_Header.hlsl"

struct HullInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};
struct ConstantType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct DomainInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;

};
struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 oPosition : POSITION0;
};
cbuffer TerrainBuffer : register(b5)
{
    int Edge;
    int Inside;
    float2 padding;

}

HullInput TerrainVS(VertexTextureNormal input)
{
    HullInput output;
    
    output.position = input.position;
    output.uv = input.uv;
    output.normal = input.normal;


    return output;
}
float2 GetScreenSpacePosition(float3 position, float4x4 viewProjection)
{
    float4 projectPosition = mul(float4(position, 1.0f), viewProjection);
    
    float2 screenPosition = projectPosition.xy / projectPosition.ww;

    screenPosition = (screenPosition + 1.0f) * 0.5f * float2(WINSIZE.x, WINSIZE.y);
    
    return screenPosition;

}

//0아래 1왼쪽 2위 3오른쪽
ConstantType PatchFunc(InputPatch<HullInput, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantType output = (ConstantType) 0;

    //float4 edge0 = (inputPatch[0].position + inputPatch[1].position) * 0.5f;
    //float4 edge1 = (inputPatch[1].position + inputPatch[2].position) * 0.5f;
    //float4 edge2 = (inputPatch[2].position + inputPatch[3].position) * 0.5f;
    //float4 edge3 = (inputPatch[3].position + inputPatch[0].position) * 0.5f;

    //float2 factor;

    //factor.x = 256 / distance(edge0.xyz, GetCameraPosition());
    //factor.y = 256 / distance(edge1.xyz, GetCameraPosition());

    
    //float inside = (factor.x + factor.y )*0.5f ;
    output.edges[0] = Edge; //factor.x; 
    output.edges[1] = Edge; 
    output.edges[2] = Edge; //factor.x;
    output.edges[3] = Edge;

    output.inside[0] = Inside; //inside;
    output.inside[1] = Inside; //inside;

    return output;
}


[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchFunc")]
[maxtessfactor(15.0)]
DomainInput TerrainHS(InputPatch<HullInput, 4> inputPatch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    DomainInput output = (DomainInput) 0;

    output.position = inputPatch[pointId].position;
    output.uv = inputPatch[pointId].uv;
    output.normal = inputPatch[pointId].normal;
    return output;
    
}


//patch  : lb , rb, lt, rt
Texture2D heightMap : register(t0);
Texture2D normalMap : register(t1);
SamplerState samp;

[domain("quad")]
PixelInput TerrainDS(ConstantType input, float2 uv : SV_DomainLocation, const OutputPatch<DomainInput, 4> patch)
{
    PixelInput output = (PixelInput) 0;

    float4 pos1 = lerp(patch[0].position, patch[1].position, uv.x);
    float4 pos2 = lerp(patch[2].position, patch[3].position, uv.x);
    float4 position = lerp(pos1, pos2, uv.y);

    float2 uv1 = lerp(patch[0].uv, patch[1].uv, uv.x);
    float2 uv2 = lerp(patch[2].uv, patch[3].uv, uv.x);
    output.uv = saturate(lerp(uv1, uv2, uv.y));

    //float3 normal1 = lerp(patch[0].normal, patch[1].normal, uv.x);
    //float3 normal2 = lerp(patch[2].normal, patch[3].normal, uv.x);
    //output.normal = saturate(lerp(normal1, normal2, uv.y));

    float2 UV = output.uv;
    position.y = heightMap.SampleLevel(samp, UV, 0).r * _heightRatio;
    float3 normal;
    normal = normalMap.SampleLevel(samp, UV, 0).rgb ;
    output.uv *= UvAmount;
    

    output.position = mul(float4(position.xyz, 1.0f), World);
    output.oPosition = output.position;

    output.position = mul(output.position, ViewProjection);

    output.normal = mul(normal, (float3x3) World);

    return output;
}


Texture2D _diffuseTex : register(t0);
Texture2D _specularTex : register(t1);
Texture2D _emissiveTex : register(t2);
Texture2D _normalTex : register(t3);
Texture2D _detailTex : register(t4);

SamplerState wrapSamp : register(s1);

/***************************************************************
Basic Deferred Shading 
****************************************************************/
G_Buffer PackGBuffer(G_Buffer buffer, float3 normal, float3 diffuse, float SpecIntensity, float SpecPower)
{
    G_Buffer Out = buffer;

	// Normalize the specular power
    float SpecPowerNorm = max(0.0001, (SpecPower - g_SpecPowerRange.x) / g_SpecPowerRange.y);

	// Pack all the data into the GBuffer structure
    Out.diffuse = float4(diffuse.rgb, SpecIntensity);
    Out.normal = float4(normal * 0.5f + 0.5f, 1.0f);
    Out.spec = float4(SpecPowerNorm, 0.0f, 0.0f, 1.0f);

    return Out;

}

Texture2D<float4> SplitMap : register(t5);
Texture2D srcTex[4] : register(t6);

float4 CalCuSplat(float4 diffuse, float2 uv)
{
    //TODO 수누꺼로 바꾸기로~

    float4 color = diffuse;

    float4 splatMap = SplitMap.Sample(wrapSamp, uv / UvAmount);
    float splat[4] = { splatMap.r, splatMap.g, splatMap.b, splatMap.a };
    
    for (int i = 0; i < 4; i++)
    {
        float4 src = srcTex[i].Sample(wrapSamp, uv);
        color += splat[i] * src;
    }

    color /= color.a + 1.0f;

    return color;
}

G_Buffer TerrainPS(PixelInput input)
{
    G_Buffer output;

    //output.worldPos = float4(SplitMap.Sample(wrapSamp, input.uv / UvAmount).rgb, 1); //input.oPosition; //  SplitMap.Sample(wrapSamp, input.uv / UvAmount);
    output.diffuse = _diffuseTex.Sample(wrapSamp, input.uv);
    //==========================TEST
    output.diffuse = CalCuSplat(output.diffuse, input.uv);
    //==============================
    output = PackGBuffer(output, input.normal,1.5f* output.diffuse.rgb, 0.25f, 250.0f);

    output.spec = _specularTex.Sample(_basicSampler, input.uv);

    output.normal.a = 0.9f;

    output.diffuse.a = 0.9f;

    return output;

}