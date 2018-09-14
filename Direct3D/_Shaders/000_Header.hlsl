/*
000_Header.hlsl
*/

cbuffer ViewProjectionBuffer : register(b0)
{
    matrix View;
    matrix Projection;
    matrix ViewProjection;
    matrix InvView;
    matrix InvProjection;
    matrix InvViewProjection;
}

cbuffer WorldBuffer : register(b1)
{
    matrix World;
}

cbuffer UnPacker : register(b2)
{
    matrix UnPackInvView;
    float4 UnPackPerspectiveValues;
}

cbuffer MaterialBuffer : register(b3)
{
    float4 AmbientColor;
    float4 DiffuseColor;
    float4 SpecColor;
    float4 EmissiveColor;

    float Shiness;
    float DetailFactor;
    float2 MaterialPadding;
}
cbuffer SunBuffer : register(b4)
{
    matrix SunView;

    float3 SunPosition;
    float SunPadding;

    float SunIntensity;
    float3 SunDir;

    float4 SunColor;

    matrix SunProjection;
    matrix SunViewProjection;
    matrix ShadowMatrix;
}


cbuffer ModelBuffer : register(b6)
{
    matrix _modelBones[230];
}



Texture2D _deferredNormal : register(t0);
Texture2D _deferredAlbedo : register(t1);
Texture2D _deferredSpecular : register(t2);
Texture2D _deferredWorld : register(t3);
Texture2D _deferredDepth : register(t4);

Texture2D _sunLightsahdowMap : register(t5);


SamplerState _basicSampler : register(s0);
SamplerComparisonState _shadowSampler : register(s2);

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

struct InstanceInputVS
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float4 blendIndices : BLENDINDICES0;
    float4 blendWeights : BLENDWEIGHTS0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;

    float4 world0 : WORLD0;
    float4 world1 : WORLD1;
    float4 world2 : WORLD2;
};


#define EyePosition (InvView[3].xyz)
#include "000_HeaderProp.hlsl"
/*************************************************************
Func
**************************************************************/

//카메라 위치 계산
float3 GetCameraPosition()
{
    return InvView[3].xyz;
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
    float linearDepth = UnPackPerspectiveValues.z / (depth + UnPackPerspectiveValues.w);
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

    position.xy = csPos.xy * UnPackPerspectiveValues.xy * depth;
    position.z = depth;
    position.w = 1.0;
	
    return mul(position, InvView).xyz;
}

float CalcShadowFactor(float4 depthPosition, Texture2D shadowMap, SamplerComparisonState shadowSampler)
{
    float2 uv;
    float3 shadowPos = depthPosition.xyz / depthPosition.w;
    uv.x = shadowPos.x * 0.5f + 0.5f;
    uv.y = shadowPos.y * -0.5f + 0.5f;

    float shadow; // = shadowMap.SampleCmpLevelZero(shadowSampler, uv, shadowPos.z).r;

    //return shadow;
    float offsetX = 1.0f / (1280.0f);
    float offsetY = 1.0f / (720.0f);

    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            shadow += shadowMap.SampleCmpLevelZero(shadowSampler, uv + float2(offsetX * j, offsetY * i), shadowPos.z).r;
        }
    }

    return clamp(shadow / 9.0f, 0.5f, 1.0f);
}


matrix DecodeMatrix(float3x4 encodedMatrix)
{
    return matrix(float4(encodedMatrix[0].xyz, 0),
                  float4(encodedMatrix[1].xyz, 0),
                  float4(encodedMatrix[2].xyz, 0),
                  float4(encodedMatrix[0].w, encodedMatrix[1].w, encodedMatrix[2].w, 1));
}

bool IntersectTri(float3 origin, float3 dir, float3 v0, float3 v1, float3 v2, out float u, out float v, out float distance)
{
    float3 edge1 = v1 - v0;
    float3 edge2 = v2 - v0;
    
    float3 pvec = cross(dir, edge2);

    float det = dot(edge1, pvec);

    //float3 temp = cross(edge1, edge2);
    //float def = dot(dir, temp);
    //if (def < 0.f)
    //    return false;
    ////==================================
    //float f = 1 / det;
    //float3 s = origin - v0;
    //u = f * dot(s, pvec);
    //if (u < 0 || u > 1)
    //    return false;

    //float3 q = cross(s, edge1);

    //v = f * dot(dir, q);
    //if (v < 0 || u + v > 1)
    //    return false;

    //distance = f * dot(edge2, q);
    //return true;


    //=====================================

    float3 tvec;
    if (det > 0.0f) 
        tvec = origin - v0;
    else
    {
        tvec = v0 - origin;
        det = -det;
    }

    if (det < 0.00001f)
        return false;

    u = dot(tvec, pvec);
    if (u < 0.0f || u > det)
        return false;

    float3 qvec = cross(tvec, edge1);
    
    v = dot(dir, qvec);
    if (v < 0.0f || u + v > det)
        return false;

    distance = dot(edge2, qvec);
    float invDet = 1 / det;
    distance *= invDet;
    u *= invDet;
    v *= invDet;

    return true;

    //===============================DX함수

    //// Find vectors for two edges sharing vert0
    //    float3 edge1 = v1 - v0;
    //    float3 edge2 = v2 - v0;

    //// Begin calculating determinant - also used to calculate U parameter
    //    float3 pvec;
    //pvec = cross(dir, edge2);

    //// If determinant is near zero, ray lies in plane of triangle
    //float det = cross(edge1, pvec);
    //float tvec;

    //    if (det > 0)
    //    {
    //    tvec = origin - v0;
    //}
    //    else
    //    {
    //    tvec = v0 - origin;
    //        det = -det;
    //    }

    //    if (det < 0.0001f)
    //        return false;

    //// Calculate U parameter and test bounds
    //u = dot(tvec, pvec);

    //    if (  u < 0.0f ||   u > det)
    //        return false;

    //// Prepare to test V parameter
    //    float qvec;
    //qvec = cross(tvec, edge1);

    //// Calculate V parameter and test bounds
    //v = dot(dir, qvec);
    //    if (  v < 0.0f ||   u +   v > det)
    //        return false;

    //// Calculate t, scale parameters, ray intersects triangle
    //distance = dot(edge2, qvec);
    //float fInvDet = 1.0f / det;
    //distance *= fInvDet;
    //u *= fInvDet;
    //v *= fInvDet;

    //    return true;
}
