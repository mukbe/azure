/*
000_Header.hlsl
*/


cbuffer VS_ViewProjection : register(b0)
{
    matrix _vsView;
    matrix _vsProjection;
    matrix _vsViewProjection;
}

cbuffer VS_World : register(b1)
{
    matrix _vsWorld;
}

cbuffer _ps_Lihgt : register(b2)
{
    float3 _lightDir;
    float _lightPadding;
}

Texture2D _deferredNormalTex : register(t0);
Texture2D _deferredPositonTex : register(t1);
Texture2D _deferredAlbedoTex : register(t2);
Texture2D _deferredDepthTex : register(t3);
Texture2D _directionalLightMap : register(t4);

SamplerState _basicSampler : register(s0);
SamplerComparisonState _shadowSampler : register(s1);

/*************************************************************
CalcShadowFactor
**************************************************************/

float CalcShadowFactor(float4 depthPosition, Texture2D shadowTex, SamplerComparisonState shadowSampler, uint pcfOn)
{
    float2 uv;
     //3D 공간에서 UV공간으로 정규화 
    float3 shadowPos = depthPosition.xyz / depthPosition.w;
    uv.x = shadowPos.x * 0.5f + 0.5f;
    uv.y = shadowPos.y * -0.5f + 0.5f;

    float shadow = 0.0f;
    float offset = 1.0f / 2048.0f;

    //PCF 미적용
    if (pcfOn > 0)
    {
        return shadowTex.SampleCmpLevelZero(shadowSampler, uv, shadowPos.z).r;
    }
    //PCF 적용
    else
    {
        for (int i = -1; i < 2; ++i)
        {
            for (int j = -1; j < 2; ++j)
            {
                shadow += shadowTex.SampleCmpLevelZero(shadowSampler, uv + float2(offset * j, offset * i), shadowPos.z).r;
            }
        }
        return shadow / 9.0f;
    }

}