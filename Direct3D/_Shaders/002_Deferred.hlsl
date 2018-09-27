#include "000_Header.hlsl"


struct BasicPixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};


BasicPixelInput BasicDeferredVS(VertexTexture input)
{
    BasicPixelInput output;

    output.position = float4(input.position.xy, 0.0f, 1.0f);
    
    output.uv = output.position.xy;
   
    return output;
}


float4 BasicDeferredPS(BasicPixelInput input) : SV_Target
{
    GBuffer_Data data = UnpackGBuffer_Loc(input.position.xy);

    float4 worldPos = float4(CalcWorldPos(input.uv, data.LinearDepth), 1.0f);
    float3 worldNormal = data.Normal;
    float3 albedo = data.DiffuseColor;
    float3 specColor = data.SpecColor;
    float specPower = data.SpecPow;


    if(data.RenderType <= 1.0f)
    {
        float4 projectionToLight = mul(worldPos, SunViewProjection);
     
        float shadowFactor = CalcShadowFactor(projectionToLight, _sunLightsahdowMap, _shadowSampler);

        float diffuseFactor = saturate(dot(worldNormal.xyz, -SunDir));
        float3 ambient = albedo * SunColor.rgb * 0.4f;
        float3 diffuseColor = albedo * diffuseFactor * SunColor.rgb;

        return float4(ambient + diffuseColor, 1.0f);
    }
    else if (data.RenderType > 1.0f && data.RenderType <= 2.0f)
    {
        return float4(albedo, 1.0f);
    }
 
   
    return float4(albedo, 1.0f);
}

