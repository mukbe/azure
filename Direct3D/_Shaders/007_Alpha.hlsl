#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};


PixelInput VS(VertexColorNormal input)
{
    PixelInput output;

    output.position = mul(input.position, World);
    output.position = mul(output.position, ViewProjection);

    output.color = input.color;

    return output;
}

float4 PS(PixelInput input) : SV_Target
{
    return input.color;
}