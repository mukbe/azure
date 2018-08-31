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

PixelInput ObjectShadowVS(InstanceInputVS input)
{
    PixelInput output;

    matrix world = DecodeMatrix(float3x4(input.world0, input.world1, input.world2));
    output.position = mul(input.position, world);
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

void ObjectShadowPS(PixelInput input)
{

}



