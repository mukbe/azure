#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
};


PixelInput VS(VertexColorNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _lightViewProjection);

    return output;
}

PixelInput TextureShadowVS(VertexTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _lightViewProjection);

    return output;
}


//ShadowMapPS
void PS(PixelInput input)
{

}

void TextureShadowPS(PixelInput input)
{

}