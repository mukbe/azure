#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
};


PixelInput ColorShadowMapVS(VertexColorNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _viewProjection);

    return output;
}

PixelInput TextureShadowVS(VertexTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _viewProjection);

    return output;
}


//ShadowMapPS
void ColorShadowMapPS(PixelInput input)
{

}

void TextureShadowPS(PixelInput input)
{

}