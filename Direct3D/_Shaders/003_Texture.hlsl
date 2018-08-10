
#include "000_Header.hlsl"

Texture2D _deferredTex[4] : register(t0);
Texture2D _depth : register(t4);
SamplerState _samp;

struct VS_INPUT
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    output.position = mul(input.position, _vsProjection);

    output.uv = input.uv;

    return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
    //return _depth.Sample(_samp, input.uv);

    float3 lightDir = float3(1, -1, 0);
    lightDir = normalize(lightDir);
    float3 normal = _deferredTex[0].Sample(_samp, input.uv) * 2.0f - 1.0f;
    float3 worldPos = _deferredTex[1].Sample(_samp,input.uv);
    float4 color = _deferredTex[2].Sample(_samp, input.uv);
    float diffuseFactor = saturate(dot(normal, -lightDir));
    float4 lightColor = float4(1, 1, 1, 1);

    color = color * lightColor * diffuseFactor;

    return color;


    return _deferredTex[0].Sample(_samp, input.uv);

}