struct VertexTexture
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};



cbuffer Font_Buffer : register(b11)
{
    matrix World;
    matrix View;
    matrix Projection;
}


PixelInput VS(VertexTexture input)
{
    PixelInput output;
    
    output.position = mul(input.position, World);
    output.position = mul(input.position, View);
    output.position = mul(input.position, Projection);
    output.uv = input.uv;


    return output;
}

Texture2D Texture : register(t5);
SamplerState TextureSampler;//: register(s5);

float4 PS(PixelInput input) : SV_Target0
{

    float4 diffuse = Texture.Sample(TextureSampler, input.uv);

    return float4(1, 1, 1, 1);
    return diffuse;
}