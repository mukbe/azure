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
Texture2D _deferredDepth : register(t4);

Texture2D _sunLightsahdowMap : register(t5);

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
    float4 color : COLOR0;
    float3 normal : NORMAL0;
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

//카메라 위치 계산
float3 GetCameraPosition()
{
    return _invView[3].xyz;
}

//노멀벡터 변환
float3 GetWorldNormal(float3 normal, float4x4 world)
{
    return mul(normal, (float3x3) world);
}

//디퓨즈 음영강도 계산
float GetDiffuseIntensity(float3 direction, float3 normal)
{
    float3 light = direction * -1;
    
    return saturate(dot(normal, light));
}

//디퓨즈 라이팅 계산
float4 GetDiffuseColor(float4 color, float3 direction, float3 normal)
{
    float3 light = direction * -1;
    float intensity = saturate(dot(normal, light));

    return float4(color.rgb * intensity, 0);
}

//스페큘러 라이팅 계산
float4 GetSpecularColor(float4 color, float3 direction, float3 normal, float3 eye, float power)
{
    //float3 reflection = reflect(direction, normal);
    //return color * pow(saturate(dot(reflection, eye)), power);

    float3 temp = normalize(direction * -1.0f + eye);
    float specular = pow(dot(normal, temp), power);

    return float4(color.rgb * specular, 0);
}

//노멀맵 공간으로 변환
float3 NormalMapSpace(float3 normalMap, float3 normal, float3 tangent)
{
    float3 unpack = 2.0f * normalMap - 1.0f;

    float3 N = normal;
    float3 T = tangent;
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

    return mul(unpack, TBN);
}

//안개량 계산
float GetFogFactor(float start, float end, float3 viewPosition)
{
    return saturate((end - viewPosition.z) / (end - start));
}

float4 GetFogColor(float4 diffuse, float4 color, float factor)
{
    return factor * diffuse + (1.0f - factor) * color;
}