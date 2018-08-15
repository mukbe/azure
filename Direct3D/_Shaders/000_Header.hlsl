/*
000_Header.hlsl
*/

cbuffer ViewProjectionBuffer : register(b0)
{
    matrix _view;
    matrix _projection;
    matrix _viewProjection;
    matrix _invView;
    matrix _invProjection;
    matrix _invViewProjection;
}

cbuffer WorldBuffer : register(b1)
{
    matrix _world;
}

cbuffer OrthoBuffer : register(b2)
{
    matrix _ortho;
}

cbuffer CameraBuffer : register(b3)
{
    float3 _cameraPos;
    float _cameraPadding;
}

cbuffer SunBuffer : register(b4)
{
    float3 _sunPosition;
    float _sunIntensity;
    float3 _sunDir;
    float _sunPadding;
    float4 _sunColor;
}

cbuffer LightViewProjectionBuffer : register(b5)
{
    matrix _lightView;
    matrix _lightProjection;
    matrix _lightViewProjection;
    matrix _shadowMatrix;
}

Texture2D _deferredNormal : register(t0);
Texture2D _deferredAlbedo : register(t1);
Texture2D _deferredSpecular : register(t2);
Texture2D _deferredWorld : register(t3);

Texture2D _sunLightsahdowMap : register(t4);

Texture2D _diffuseTex : register(t5);
Texture2D _specularTex : register(t6);

SamplerState _basicSampler : register(s0);

/*************************************************************
Struct
**************************************************************/

struct VertexColor
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct VertexColorNormal
{
    float4 position : POSITION0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};



struct VertexTexture
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct VertexTextureNormal
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct G_Buffer
{
    float4 normal : SV_Target0;
    float4 diffuse : SV_Target1;
    float4 spec : SV_Target2;
    float4 worldPos : SV_Target3;
};


/*************************************************************
Func
**************************************************************/

float3 normalPacking(in float3 normal)
{
    return normal * 0.5f + 0.5f;
}

float3 normalUnpacking(in float3 normal)
{
    return normal * 2.0f - 1.0f;
}