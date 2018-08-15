#include "000_Header.hlsl"

struct BasicPixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

BasicPixelInput BasicDeferredVS(VertexTexture input)
{
    BasicPixelInput output;

    output.position = mul(input.position, _ortho);
    output.uv = input.uv;

    return output;
}

float4 BasicDeferredPS(BasicPixelInput input) : SV_Target
{
    //GetGBufferData
    float4 worldPos = _deferredWorld.Sample(_basicSampler, input.uv);
    float3 worldNormal = normalUnpacking(_deferredNormal.Sample(_basicSampler, input.uv).xyz);
    float4 albedo = _deferredAlbedo.Sample(_basicSampler, input.uv);
    float4 specPower = _deferredSpecular.Sample(_basicSampler, input.uv);

    //GizmoRendering
    if (specPower.w > 1.5f)
        return albedo;


    //CalcMainShadow
    float4 posLight = mul(worldPos, _shadowMatrix);
    float2 samplingShadowData = _sunLightsahdowMap.Sample(_basicSampler, posLight.xy).xw;
    float shadowMapDepth = samplingShadowData.x / samplingShadowData.y;

    float4 projectionToLight = mul(worldPos, _lightViewProjection);
    float pixelDepth = projectionToLight.x / projectionToLight.w;


    //CalcLighting
    float diffuseFactor = saturate(dot(worldNormal.xyz, -_sunDir));
    float4 diffuseColor = albedo * diffuseFactor * _sunColor;
 
    //if in Shadow
    if (shadowMapDepth > pixelDepth)
    {
        //TODO ºû°è»ê
        return diffuseColor * shadowMapDepth;

    }
    else
    {
        return diffuseColor;
    }
}
