
Texture2D uiTex : register(t0);

SamplerState basicSampler;

struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

cbuffer OrthoBuffer : register(b10)
{
    matrix ortho;
}

PixelInput ButtonVS(VertexInput input)
{
    PixelInput output;

    output.position = mul(float4(input.position.xy, 0.0f, 1.0f), ortho);

    output.uv = input.uv;
   
    return output;
}

float4 ButtonPS(PixelInput input) : SV_Target
{
    return uiTex.Sample(basicSampler, input.uv);
}