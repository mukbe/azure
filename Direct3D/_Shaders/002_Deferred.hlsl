#include "000_Header.hlsl"

static const float albedoBias = 0.5f;
static const float shadowBias = 0.08f;

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

cbuffer DeferredBuffer : register(b10)
{
    float ShadowBias;
    float3 ShadowPadding;
}

//PCF(Percentage Closer Filtering) Shadow
float CalcShadowFactor(float4 depthPosition, Texture2D shadowMap, SamplerComparisonState shadowSampler)
{
    float2 uv;
    float3 shadowPos = depthPosition.xyz / depthPosition.w;
    uv.x = shadowPos.x * 0.5f + 0.5f;
    uv.y = shadowPos.y * -0.5f + 0.5f;

    float shadow = 0.0f;

    float offsetX = 1.0f / WINSIZE.x;
    float offsetY = 1.0f / WINSIZE.y;

    [unroll]
    for (int i = -1; i < 2; ++i)
    {
        [unroll]
        for (int j = -1; j < 2; ++j)
        {
            shadow += shadowMap.SampleCmpLevelZero(shadowSampler, uv + float2(offsetX * j, offsetY * i), shadowPos.z + ShadowBias).r;
        }
    }

    return clamp(shadow / 9.0f, 0.0f, 1.0f);
}

float4 BasicDeferredPS(BasicPixelInput input) : SV_Target
{
    GBuffer_Data data = UnpackGBuffer_Loc(input.position.xy);

    float4 worldPos = float4(CalcWorldPos(input.uv, data.LinearDepth), 1.0f);
    float3 worldNormal = data.Normal;
    float3 albedo = data.DiffuseColor;
    float3 specColor = data.SpecColor;
    float specPower = data.SpecPow;

     //Default Shading -------------------------------------------------------------------------------------------------
    if(data.RenderType <= 1.0f)    
    {
        float4 projectionToLight = mul(worldPos, SunViewProjection);
        float shadowFactor = CalcShadowFactor(projectionToLight, _sunLightsahdowMap, _shadowSampler);

        float diffuseFactor = saturate(dot(worldNormal.xyz, -SunDir));
        float3 ambient = albedo * SunColor.rgb * albedoBias;
        float3 diffuseColor = albedo * SunColor.rgb * diffuseFactor; //* shadowFactor;
        float3 specColor = float3(0, 0, 0);
       if(diffuseFactor > 0.0f)
        {
            float3 reflection = normalize(2.0f *  worldNormal - (-SunDir));
            specColor = data.SpecColor * SunColor.rgb * 
                saturate(pow(saturate(dot(reflection, normalize(GetCameraPosition() - worldPos.xyz))), specPower));
        }

        float4 outputColor = float4(ambient + diffuseColor * shadowFactor + specColor * shadowFactor, 1.0f);
        float fogFactor = GetFogFactor(FogStart, FogEnd, mul(worldPos, View).xyz);
        return GetFogColor(outputColor, FogColor, fogFactor);
    }
    //------------------------------------------------------------------------------------------------------
    //Debug Shading-----------------------------------------------------------------------------------------
    else if (data.RenderType > 1.0f && data.RenderType <= 2.0f)     
    {
        float4 outputColor = float4(albedo, 1.0f);
        float fogFactor = GetFogFactor(FogStart, FogEnd, mul(worldPos, View).xyz);
        return GetFogColor(outputColor, FogColor, fogFactor);
        //return float4(albedo, 1.0f);
    }
    //-------------------------------------------------------------------------------------------------------
    //Ocean Shading -----------------------------------------------------------------------------------------
    else if(data.RenderType > 2.0f && data.RenderType <= 3.0f)      
    {
        float3 sunDirection = SunDir;
    
        float3 V = normalize(GetCameraPosition() - worldPos.xyz);
        float3 N = normalize(data.Normal);
	
        float fresnel = GetFresnel(V, N);
        float3 specColor = lerp(albedo, SunColor.rgb, fresnel);
    
        float diffuseFactor = saturate(dot(data.Normal, -sunDirection));
        float3 diffuseColor = (albedo * SunColor.rgb * diffuseFactor).rgb;

        float4 outputColor = float4(diffuseColor + specColor, 1.0f);
        float fogFactor = GetFogFactor(FogStart, FogEnd, mul(worldPos, View).xyz);
        return GetFogColor(outputColor, FogColor, fogFactor);

        //return float4(diffuseColor + specColor, 1.0f);
    }
    //--------------------------------------------------------------------------------------------------------
    //Grass Shading ------------------------------------------------------------------------------------------
    else if(data.RenderType > 3.0f && data.RenderType <= 4.0f)      
    {
        float4 projectionToLight = mul(worldPos, SunViewProjection);
        float shadowFactor = CalcShadowFactor(projectionToLight, _sunLightsahdowMap, _shadowSampler);
        float3 ambient = albedo * SunColor.rgb * albedoBias;

        float4 outputColor = float4(ambient + albedo * SunColor.rgb * shadowFactor, 1.0f);
        float fogFactor = GetFogFactor(FogStart, FogEnd, mul(worldPos, View).xyz);
        return GetFogColor(outputColor, FogColor, fogFactor);

        //return float4(ambient + albedo * SunColor.rgb * shadowFactor, 1.0f);
    }
    //---------------------------------------------------------------------------------------------------------


       return float4(albedo, 1.0f);
}

