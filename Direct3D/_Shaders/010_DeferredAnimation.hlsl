#include "000_Header.hlsl"

struct ModelPixelInput
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float depth : TEXCOOORD1;
};



Texture2D _diffuseTex : register(t0);
Texture2D _specularTex : register(t1);


static const int UseDeiffuseMap = 1;
static const int UseNormalMap = 2;
static const int UseSpecularMap = 4;
static const int UseEmissiveMap = 8;

StructuredBuffer<matrix> skinTransformBuffer : register(t10);

G_Buffer PackGBuffer(G_Buffer buffer, float3 normal, float3 diffuse, float3 specColor,
    float depth, float SpecPower, float renderType)
{
    G_Buffer Out = buffer;

    float SpecPowerNorm = max(0.0001, (SpecPower - g_SpecPowerRange.x) / g_SpecPowerRange.y);

    Out.normal = float4(normal * 0.5f + 0.5f, renderType);
    Out.diffuse = float4(diffuse.rgb, depth);
    Out.spec = float4(specColor, SpecPower);

    return Out;

}

ModelPixelInput VS(VertexTextureBlendNT input)
{
    ModelPixelInput output;

    float4x4 transform = 0;
    transform += mul(input.blendWeights.x, skinTransformBuffer[(uint) input.blendIndices.x]);
    transform += mul(input.blendWeights.y, skinTransformBuffer[(uint) input.blendIndices.y]);
    transform += mul(input.blendWeights.z, skinTransformBuffer[(uint) input.blendIndices.z]);
    transform += mul(input.blendWeights.w, skinTransformBuffer[(uint) input.blendIndices.w]);

    output.position = output.worldPos = mul(input.position, transform);
   
    output.normal = mul(input.normal, (float3x3) transform);
    output.tangent = mul(input.tangent, (float3x3) transform);

    output.position = mul(output.position, ViewProjection);

    output.uv = input.uv;

    output.depth = output.position.z / output.position.w;

    return output;
}


G_Buffer PS(ModelPixelInput input)
{
    G_Buffer output;

    float3 diffuse = _diffuseTex.Sample(_basicSampler, input.uv).rgb;
    output = PackGBuffer(output, normalize(input.normal), diffuse, SpecColor.rgb, 1.0f, Shiness, 0.9f);
    return output;
}