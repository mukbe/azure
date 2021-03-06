#include "000_Header.hlsl"


//MRT0 Normal.xyz, RenderType(float)
//MRT1 Diffuse.rgb,depth
//MRT2 Specr.rgb, SpecPower(float)
//MRT3 DepthMap

//RenderType -- 0.0f ~ 0.9f == 빛계산 함
//RenderType -- 1.0f ~ 1.9f == 빛계산 안함(GBuffer로 넘어온 Diffuse출력) 

G_Buffer PackGBuffer(G_Buffer buffer, float3 normal, float3 diffuse, float3 specColor,
    float depth, float SpecPower, float renderType)
{
    G_Buffer Out = buffer;

	// Normalize the specular power
    float SpecPowerNorm = max(0.0001, (SpecPower - g_SpecPowerRange.x) / g_SpecPowerRange.y);

	// Pack all the data into the GBuffer structure
    Out.normal = float4(normal * 0.5f + 0.5f, renderType);
    Out.diffuse = float4(diffuse.rgb, 1.0f);
    Out.spec = float4(specColor, SpecPowerNorm);

    return Out;

}

Texture2D _grassTexture[5] : register(t0);

struct VS_INPUT
{
    float4 position : POSITION0;
    float2 scale : TEXCOORD0;
    float3 normal : NORMAL0;
    uint textureIndex : TEXCOORD1;
};

struct PS_INPUT
{ 
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    uint textureIndex : TEXCOORD1;
};


VS_INPUT VS(VS_INPUT input)
{
    return input;
}

[maxvertexcount(4)]
void GS(point VS_INPUT gin[1],
            inout TriangleStream<PS_INPUT> triStream)
{
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = normalize(gin[0].normal);
    float3 right = normalize(cross(up, look));

    float halfWidth = gin[0].scale.x / 2.0f;
    float halfHeight = gin[0].scale.y;
    float3 position = gin[0].position;

    float4 v[4];
    float2 uv[4];
    v[0] = float4(position + halfWidth * right, 1.0f);
    uv[0] = float2(1.f, 1.f);
    v[1] = float4(position + halfWidth * right + halfHeight * up, 1.0f);
    uv[1] = float2(1.f, 0.f);
    v[2] = float4(position - halfWidth * right, 1.0f);
    uv[2] = float2(0.f, 1.f);
    v[3] = float4(position - halfWidth * right + halfHeight * up, 1.0f);
    uv[3] = float2(0.f, 0.f);

	[unroll]
    for (int i = 0; i < 4; ++i)
    {
        PS_INPUT gout;
        gout.position = mul(v[i], ViewProjection);
        gout.uv = uv[i];
        gout.normal = look;
        gout.textureIndex = gin[0].textureIndex;

        triStream.Append(gout);
    }
}

G_Buffer PS(PS_INPUT input)
{
    G_Buffer output;
    float4 diffuseSample = float4(1, 0, 0, 1);
    
    if (input.textureIndex == 0)
        diffuseSample = _grassTexture[0].Sample(_basicSampler, input.uv);
    else if(input.textureIndex == 1)
        diffuseSample = _grassTexture[1].Sample(_basicSampler, input.uv);
    else if(input.textureIndex == 2)
        diffuseSample = _grassTexture[2].Sample(_basicSampler, input.uv);
    else if(input.textureIndex == 3)
        diffuseSample = _grassTexture[3].Sample(_basicSampler, input.uv);
    else if(input.textureIndex == 4)
        diffuseSample = _grassTexture[4].Sample(_basicSampler, input.uv);

    //diffuseSample = float4(0, 0, 1, 1);
    if (diffuseSample.a < 0.1f)
        discard;
    diffuseSample = diffuseSample * 0.7f;

    output = PackGBuffer(output, input.normal, diffuseSample.rgb, float3(0, 0, 0), 0, 0, 3.5f);

    return output;
}


/**********************************************************************************************
Billboard Grass Shadow
**********************************************************************************************/

VS_INPUT GrassShadowVS(VS_INPUT input)
{
    return input;
}

[maxvertexcount(4)]
void GrassShadowGS(point VS_INPUT gin[1],
            inout TriangleStream<PS_INPUT> triStream)
{
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = normalize(gin[0].normal);
    float3 right = normalize(cross(up, look));

    float halfWidth = gin[0].scale.x / 2.0f;
    float halfHeight = gin[0].scale.y;
    float3 position = gin[0].position;

    float4 v[4];
    float2 uv[4];
    v[0] = float4(position + halfWidth * right, 1.0f);
    uv[0] = float2(1.f, 1.f);
    v[1] = float4(position + halfWidth * right + halfHeight * up, 1.0f);
    uv[1] = float2(1.f, 0.f);
    v[2] = float4(position - halfWidth * right, 1.0f);
    uv[2] = float2(0.f, 1.f);
    v[3] = float4(position - halfWidth * right + halfHeight * up, 1.0f);
    uv[3] = float2(0.f, 0.f);

	[unroll]
    for (int i = 0; i < 4; ++i)
    {
        PS_INPUT gout;
        gout.position = mul(v[i], SunViewProjection);
        gout.uv = uv[i];
        gout.normal = look;
        gout.textureIndex = gin[0].textureIndex;

        triStream.Append(gout);
    }
}

void GrassShadowPS(PS_INPUT input)
{
    float4 diffuseSample = float4(1, 0, 0, 1);
    if (input.textureIndex == 0)
        diffuseSample = _grassTexture[0].Sample(_basicSampler, input.uv);
    else if (input.textureIndex == 1)
        diffuseSample = _grassTexture[1].Sample(_basicSampler, input.uv);
    else if (input.textureIndex == 2)
        diffuseSample = _grassTexture[2].Sample(_basicSampler, input.uv);
    else if (input.textureIndex == 3)
        diffuseSample = _grassTexture[3].Sample(_basicSampler, input.uv);
    else if (input.textureIndex == 4)
        diffuseSample = _grassTexture[4].Sample(_basicSampler, input.uv);

    //diffuseSample = float4(0, 0, 1, 1);
    diffuseSample = diffuseSample * 0.6f;
    if (diffuseSample.a < 0.1f)
        discard;

}