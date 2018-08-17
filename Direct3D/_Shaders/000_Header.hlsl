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

cbuffer UnPacker : register(b2)
{
    matrix InvView;
    float4 PerspectiveValues;
}

cbuffer MaterialBuffer : register(b3)
{
    float4 _diffuseColor;
    float4 _specColor;
    float4 _emissiveColor;

    float _shiness;
    float _detailFactor;
    float2 _materialPadding;
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

cbuffer ModelBuffer : register(b6)
{
    matrix _modelBones[130];
}



Texture2D _deferredNormal : register(t0);
Texture2D _deferredAlbedo : register(t1);
Texture2D _deferredSpecular : register(t2);
Texture2D _deferredWorld : register(t3);
Texture2D _deferredDepth : register(t4);

Texture2D _sunLightsahdowMap : register(t5);


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

struct VertexTextureBlendNT
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float4 blendIndices : BLENDINDICES0;
    float4 blendWeights : BLENDWEIGHTS0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
};

struct G_Buffer
{
    float4 normal : SV_Target0;
    float4 diffuse : SV_Target1;
    float4 spec : SV_Target2;
    float4 worldPos : SV_Target3;
};
struct GBuffer_Data
{
    float LinearDepth;
    float4 WorldPosition;
    float3 Color;
    float3 Normal;
    float SpecPow;
    float SpecIntensity;
};


#define EyePosition (_invView[3].xyz)

static const float2 g_SpecPowerRange = { 10.0, 250.0 };

/*************************************************************
Func
**************************************************************/



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

float ConvertZToLinearDepth(float depth)
{
    float linearDepth = PerspectiveValues.z / (depth + PerspectiveValues.w);
    return linearDepth;
}

GBuffer_Data UnpackGBuffer(float2 uv)
{
    GBuffer_Data Out;

    float depth = _deferredDepth.Sample(_basicSampler, uv).x;
    Out.LinearDepth = ConvertZToLinearDepth(depth);
    float4 baseColorSpecInt = _deferredAlbedo.Sample(_basicSampler, uv);
    Out.Color = baseColorSpecInt.xyz;
    Out.SpecIntensity = baseColorSpecInt.w;
    Out.Normal = _deferredNormal.Sample(_basicSampler, uv).xyz;
    Out.Normal = normalize(Out.Normal * 2.0 - 1.0);
    Out.SpecPow = _deferredSpecular.Sample(_basicSampler, uv).x;
    Out.WorldPosition = _deferredWorld.Sample(_basicSampler, uv);

    return Out;
}

GBuffer_Data UnpackGBuffer_Loc(int2 location)
{
    GBuffer_Data Out;
    int3 location3 = int3(location, 0);

    float depth = _deferredDepth.Load(location3).x;
    Out.LinearDepth = ConvertZToLinearDepth(depth);
    float4 baseColorSpecInt = _deferredAlbedo.Load(location3);
    Out.Color = baseColorSpecInt.xyz;
    Out.SpecIntensity = baseColorSpecInt.w;
    Out.Normal = _deferredNormal.Load(location3).xyz;
    Out.Normal = normalize(Out.Normal * 2.0 - 1.0);
    Out.SpecPow = _deferredSpecular.Load(location3).x;
    Out.WorldPosition = _deferredWorld.Load(location3);

    return Out;
}

float3 CalcWorldPos(float2 csPos, float depth)
{
    float4 position;

    position.xy = csPos.xy * PerspectiveValues.xy * depth;
    position.z = depth;
    position.w = 1.0;
	
    return mul(position, InvView).xyz;
}