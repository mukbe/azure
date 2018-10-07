#include "000_Header.hlsl"

struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;

    float2 offset : TEXCOORD1;
};

struct PixelInput
{
    float4 position : SV_POSITION0;
    float3 normal : NORMAL0;
    float3 color : TEXCOORD0;
};

struct oceanData
{
    float3 position;
    float2 uv;
    float3 normal;
};

Texture2D _fresnelLookUpTable : register(t6);
StructuredBuffer<oceanData> _heightBuffer : register(t7);


PixelInput InstanceVS(VertexInput input)
{
    PixelInput output;

    uint index = input.position.z * 65 + input.position.x;
    oceanData data = _heightBuffer[index];

    float4 worldPosition = mul(float4(data.position, 1.0f) + float4(input.offset.x, 0.0f, input.offset.y, 0.f), World);
    float3 worldPos = worldPosition.xyz;

    output.position = mul(worldPosition, ViewProjection);
    output.color = DiffuseColor;
    output.normal = normalize(data.normal);

    return output;
}



G_Buffer InstancePS(PixelInput input)
{
    G_Buffer output;

    output.normal = float4(input.normal * 0.5f + 0.5f, 2.5f);
    output.diffuse = float4(input.color, 1.0f);
    output.spec = float4(1, 1, 1, 1);

    return output;
}

