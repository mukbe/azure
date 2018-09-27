#include "000_Header.hlsl"

Texture2D _diffuseTex : register(t0);
Texture2D _specularTex : register(t1);
Texture2D _emissiveTex : register(t2);
Texture2D _normalTex : register(t3);
Texture2D _detailTex : register(t4);

static const int UseDeiffuseMap = 1;
static const int UseNormalMap = 2;
static const int UseSpecularMap = 4;
static const int UseEmissiveMap = 8;

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
    Out.diffuse = float4(diffuse.rgb, depth);
    Out.spec = float4(specColor, SpecPowerNorm);

    return Out;

}

/****************************************************************
Basic Deferred Shading 
****************************************************************/

struct BasicPixelInput
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

BasicPixelInput BasicDeferredVS(VertexTextureNormal input)
{
    BasicPixelInput output;

    output.position = output.worldPos = mul(input.position, World);
    output.position = mul(output.position, ViewProjection);

    output.normal = normalize(mul(input.normal, (float3x3) World));

    output.uv = input.uv;

    return output;
}


G_Buffer BasicDeferredPS(BasicPixelInput input)
{
    G_Buffer output;

    //output.worldPos = input.worldPos;
    output.diffuse = _diffuseTex.Sample(_basicSampler, input.uv);
    output = PackGBuffer(output, input.normal, output.diffuse.rgb, float3(1, 1, 1),1.0f,1.0f,0.5f);

    output.spec = _specularTex.Sample(_basicSampler, input.uv);

    return output;

}



/***************************************************************
Color Deferred Shading 
****************************************************************/

struct ColorNormalPixelInput
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};



ColorNormalPixelInput ColorDeferredVS(VertexColorNormal input)
{
    ColorNormalPixelInput output;

    output.position = output.worldPos = mul(input.position, World);
    output.position = mul(output.position, ViewProjection);

    output.normal = mul(input.normal, (float3x3) World);

    output.color = input.color;

    return output;
}

G_Buffer ColorDeferredPS(ColorNormalPixelInput input)
{
    G_Buffer output;

    //output.worldPos = input.worldPos;
    output.diffuse = input.color;
    output = PackGBuffer(output, input.normal, output.diffuse.rgb, float3(1, 1, 1),1.0f, 1.0f,1.5f);

    return output;
}

/****************************************************************
Gizmo Deferred Shading 
****************************************************************/

struct ColorPixelInput
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION0;
    float4 color : COLOR0;
};


ColorPixelInput GizmoDeferredVS(VertexColorNormal input)
{
    ColorPixelInput output;

    output.position = output.worldPos = mul(input.position, World);
    output.position = mul(output.position, ViewProjection);

    output.color = input.color;

    return output;
}

G_Buffer GizmoDeferredPS(ColorPixelInput input)
{
    G_Buffer output;

    output = PackGBuffer(output, float3(0, 0, 0), input.color.rgb, float3(0, 0, 0), 1.0f, 1.0f, 1.5f);

    return output;
}

//===================================================
//Deferred Model
//===================================================

struct ModelPixelInput
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float depth : TEXCOOORD1;
};


ModelPixelInput ModelDeferredVS(VertexTextureBlendNT input)
{
    ModelPixelInput output;

    float4x4 transform = 0;
    transform += mul(input.blendWeights.x, _modelBones[(uint) input.blendIndices.x]);
    transform += mul(input.blendWeights.y, _modelBones[(uint) input.blendIndices.y]);
    transform += mul(input.blendWeights.z, _modelBones[(uint) input.blendIndices.z]);
    transform += mul(input.blendWeights.w, _modelBones[(uint) input.blendIndices.w]);

    output.position = output.worldPos = mul(input.position, transform);
    //output.position = output.worldPos = input.position;
   
    output.normal = mul(input.normal, (float3x3) transform);
    output.tangent = mul(input.tangent, (float3x3) transform);

    output.position = mul(output.position, ViewProjection);

    output.uv = input.uv;

    output.depth = output.position.z / output.position.w;

    return output;
}


G_Buffer ModelDeferredPS(ModelPixelInput input)
{
    G_Buffer output;

    float3 diffuse = _diffuseTex.Sample(_basicSampler, input.uv).rgb;
    

    output.normal = float4(NormalMapSpace(_normalTex.Sample(_basicSampler, input.uv).xyz, input.normal, input.tangent), 1);
    output.spec = float4(1, 1, 1, 2);

    output = PackGBuffer(output, input.normal, diffuse,SpecColor.rgb,input.depth,Shiness,0.5f);
    return output;
}

//===================================================
//Deferred Instancing Object
//===================================================

ModelPixelInput InstanceVS(InstanceInputVS input)
{
    ModelPixelInput output;
    matrix worldMatrix = DecodeMatrix(float3x4(input.world0, input.world1, input.world2));
    matrix finalMatrix = worldMatrix;

    output.position = output.worldPos = mul(input.position, finalMatrix);

    output.normal = normalize(mul(input.normal, (float3x3) finalMatrix));
    output.tangent = mul(input.tangent, (float3x3) finalMatrix);

    output.position = mul(output.position, ViewProjection);
    
    output.uv = input.uv;

    output.depth = output.position.z / output.position.w;

    return output;
}


G_Buffer InstancePS(ModelPixelInput input)
{
    G_Buffer output;

    float4 diffuse4 = _diffuseTex.Sample(_basicSampler, input.uv);

    if (diffuse4.a < 0.1f)
        discard;

    float3 normal;
    float specPower;

    if (TextureCheck & UseNormalMap)
        normal = normalize(NormalMapSpace(_normalTex.Sample(_basicSampler, input.uv).xyz, input.normal, input.tangent));
    else 
        normal = normalize(input.normal);

    if(TextureCheck & UseSpecularMap)
        specPower = _specularTex.Sample(_basicSampler, input.uv).r;
    else 
        specPower = Shiness;

    output = PackGBuffer(output, normal, diffuse4.rgb, SpecColor.rgb, input.depth, specPower, 0.9f);
    return output;
}
//===================================================
//Deferred UnInstance Object
//===================================================

ModelPixelInput ObjectVS(VertexTextureBlendNT input)
{
    ModelPixelInput output;

    output.position = output.worldPos = mul(input.position, World);

    output.normal = mul(input.normal, (float3x3) World);
    output.tangent = mul(input.tangent, (float3x3) World);

    output.position = mul(output.position, ViewProjection);

    output.uv = input.uv;

    return output;
}

G_Buffer ObjectPS(ModelPixelInput input)
{
    G_Buffer output;

    float3 diffuse = _diffuseTex.Sample(_basicSampler, input.uv).rgb;


    //output.worldPos = input.worldPos;
    output.normal = float4(NormalMapSpace(_normalTex.Sample(_basicSampler, input.uv).xyz, input.normal, input.tangent), 1);
    output.spec = float4(1, 1, 1, 2);

    output = PackGBuffer(output, input.normal, diffuse,SpecColor.rgb,SpecColor.a,Shiness,0.5f);
    return output;
}