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

G_Buffer PackGBuffer(G_Buffer buffer, float3 normal, float3 diffuse, float3 specColor,
    float SpecIntensity, float SpecPower, float renderType)
{
    G_Buffer Out = buffer;

    float SpecPowerNorm = max(0.0001, (SpecPower - g_SpecPowerRange.x) / g_SpecPowerRange.y);

    Out.normal = float4(normal * 0.5f + 0.5f, renderType);
    Out.diffuse = float4(diffuse.rgb, SpecIntensity);
    Out.spec = float4(specColor, SpecPowerNorm);

    return Out;

}

PixelInput InstanceVS(VertexInput input)
{
    PixelInput output;

    float4 worldPosition = mul(input.position + float4(input.offset.x, 0, input.offset.y, 0.f), World);
    output.position = mul(worldPosition, ViewProjection);
    output.color = DiffuseColor;
    output.normal = input.normal;


    return output;
}


G_Buffer InstancePS(PixelInput input)
{
    G_Buffer output;
    float3 normal = normalize(input.normal);
    output.normal = float4(normal * 2.0f - 1.0f, 2.5f);
    output.diffuse = float4(input.color, 1.0f);
    output.spec = float4(1, 1, 1, 1);

    return output;
}
