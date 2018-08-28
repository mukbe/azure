#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
};


PixelInput ColorShadowVS(VertexColorNormal input)
{
    PixelInput output;

    output.position = mul(input.position, World);
    output.position = mul(output.position, SunViewProjection);

    return output;
}

PixelInput TextureShadowVS(VertexTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, World);
    output.position = mul(output.position, SunViewProjection);

    return output;
}


//ShadowMapPS
void ColorShadowPS(PixelInput input)
{

}

void TextureShadowPS(PixelInput input)
{

}