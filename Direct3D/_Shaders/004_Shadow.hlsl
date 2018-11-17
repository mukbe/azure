#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
};

struct ObjectPixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D _diffuseTex : register(t0);


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

ObjectPixelInput InstanceVS(InstanceInputVS input)
{
    ObjectPixelInput output;

    matrix world = DecodeMatrix(float3x4(input.world0, input.world1, input.world2));
    output.position = mul(input.position, world);
    output.position = mul(output.position, SunViewProjection);

    output.uv = input.uv;
    
    return output;
}

struct ModelPixelInput
{
    float4 position : SV_POSITION;
};


ModelPixelInput ModelDeferredVS(VertexTextureBlendNT input)
{
    ModelPixelInput output;

    float4x4 transform = 0;
    transform += mul(input.blendWeights.x, _modelBones[(uint) input.blendIndices.x]);
    transform += mul(input.blendWeights.y, _modelBones[(uint) input.blendIndices.y]);
    transform += mul(input.blendWeights.z, _modelBones[(uint) input.blendIndices.z]);
    transform += mul(input.blendWeights.w, _modelBones[(uint) input.blendIndices.w]);
    output.position = mul(input.position, transform);
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

void InstancePS(ObjectPixelInput input)
{
    float4 diffuse4 = _diffuseTex.Sample(_basicSampler, input.uv);

    if (diffuse4.a < 0.1f)
        discard;
}



void ModelDeferredPS(ModelPixelInput input)
{
  
}
