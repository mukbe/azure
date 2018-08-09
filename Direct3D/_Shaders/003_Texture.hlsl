
cbuffer VS_ViewProjection : register(b0)
{
    matrix _vsView;
    matrix _vsProjection;
    matrix _vsViewProjection;
}

cbuffer VS_WorldBuffer : register(b1)
{
    matrix _vsWorld;
}

Texture2D _deferredTex[4] : register(t0);
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
    return _deferredTex[3].Sample(_samp, input.uv);

}