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

}

HullInput TerrainShadowVS(VertexTextureNormal input)
{
    HullInput output;
    
    output.position = input.position;
    output.uv = input.uv;
    output.normal = input.normal;


    return output;
}

ConstantType PatchFunc(InputPatch<HullInput, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantType output = (ConstantType) 0;
    output.edges[0] = Edge; //edge;
    output.edges[1] = Edge; //edge;
    output.edges[2] = Edge; //edge;
    output.edges[3] = Edge; //edge;

    output.inside[0] = Inside; //inside;
    output.inside[1] = Inside; //inside;
    
    return output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchFunc")]
DomainInput TerrainShadowHS(InputPatch<HullInput, 4> inputPatch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
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
SamplerState samp : register(s0);

[domain("quad")]
PixelInput TerrainShadowDS(ConstantType input, float2 uv : SV_DomainLocation, const OutputPatch<DomainInput, 4> patch)
{
    PixelInput output = (PixelInput) 0;

    float4 pos1 = lerp(patch[0].position, patch[1].position, uv.x);
    float4 pos2 = lerp(patch[2].position, patch[3].position, uv.x);
    float4 position = lerp(pos1, pos2, uv.y);

    float2 uv1 = lerp(patch[0].uv, patch[1].uv, uv.x);
    float2 uv2 = lerp(patch[2].uv, patch[3].uv, uv.x);
    output.uv = saturate(lerp(uv1, uv2, uv.y));


    float2 UV = output.uv;
    position.y = heightMap.SampleLevel(samp, UV, 0).r * _heightRatio;

    
    output.position = mul(float4(position.xyz, 1.0f), World);
    output.oPosition = output.position;
    output.position = mul(output.position, SunViewProjection);
    output.normal = float3(0, 0, 0);

    return output;
}




void TerrainShadowPS(PixelInput input)
{

}