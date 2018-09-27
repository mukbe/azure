#include "000_Header.hlsl"

struct HullInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 BrushColor : COLOR0;
};
struct ConstantType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct DomainInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 BrushColor : COLOR0;

};
struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 BrushColor : COLOR0;
    float4 oPosition : POSITION0;

};
cbuffer TerrainTool : register(b5)
{
    float2 MouseScreenPos;
    int BrushStyle;
    float BrushSize;

    float3 PickPos;
    float HeightAmount;

    uint GridbView;
    float GridThickness;
    float2 padding;

}

float4 GetBrushColor(float3 pos)
{
    if (BrushStyle == 0)
    {
        if (pos.x >= (PickPos.x - BrushSize) &&
          pos.x <= (PickPos.x + BrushSize) &&
          pos.z >= (PickPos.z - BrushSize) &&
          pos.z <= (PickPos.z + BrushSize))
        {
            return float4(0, 0, 1, 1);
        }
    }
    if (BrushStyle == 1)
    {
        float dx = pos.x - PickPos.x;
        float dy = pos.z - PickPos.z;

        float distance = sqrt(dx * dx + dy * dy);

        if (distance <= BrushSize)
        {
            return float4(0, 0, 1, 1);
        }

    }
    return float4(0, 0, 0, 0);
}

HullInput TerrainToolVS(VertexTextureNormal input)
{
    HullInput output;
    
    output.position = input.position;
    output.uv = input.uv;
    output.normal = input.normal;
    output.BrushColor = GetBrushColor(input.position.xyz); // GetBrushColor(input.position.xyz);


    return output;
}

ConstantType PatchFunc(InputPatch<HullInput, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantType output = (ConstantType) 0;
    output.edges[0] = 4; //edge;
    output.edges[1] = 4; //edge;
    output.edges[2] = 4; //edge;
    output.edges[3] = 4; //edge;

    output.inside[0] = 2;//inside;
    output.inside[1] = 2;//inside;
    
    //bool bViewFrustumCull = ViewFrustumCull(inputPatch[0].position.xyz, inputPatch[1].position.xyz,
    //        inputPatch[2].position.xyz, inputPatch[3].position.xyz, _frustum, 20.0f);

    //if (bViewFrustumCull)
    //{
    //    // Set all tessellation factors to 0 if frustum cull test succeeds
    //    output.edges[0] = 0.0;
    //    output.edges[1] = 0.0;
    //    output.edges[2] = 0.0;
    //    output.edges[3] = 0.0;
    //    output.inside[0] = 0.0;
    //    output.inside[1] = 0.0;
    //}




    return output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchFunc")]
DomainInput TerrainToolHS(InputPatch<HullInput, 4> inputPatch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    DomainInput output = (DomainInput) 0;

    output.position = inputPatch[pointId].position;
    output.uv = inputPatch[pointId].uv;
    output.normal = inputPatch[pointId].normal;
    output.BrushColor = inputPatch[pointId].BrushColor;
    return output;
    
}


//patch  : lb , rb, lt, rt
Texture2D map : register(t0);
SamplerState samp;

[domain("quad")]
PixelInput TerrainToolDS(ConstantType input, float2 uv : SV_DomainLocation, const OutputPatch<DomainInput, 4> patch)
{
    PixelInput output = (PixelInput) 0;

    float4 pos1 = lerp(patch[0].position, patch[1].position, uv.x);
    float4 pos2 = lerp(patch[2].position, patch[3].position, uv.x);
    float4 position = lerp(pos1, pos2, uv.y);

    float2 uv1 = lerp(patch[0].uv, patch[1].uv, uv.x);
    float2 uv2 = lerp(patch[2].uv, patch[3].uv, uv.x);
    output.uv = saturate(lerp(uv1, uv2, uv.y));

    float3 normal1 = lerp(patch[0].normal, patch[1].normal, uv.x);
    float3 normal2 = lerp(patch[2].normal, patch[3].normal, uv.x);
    output.normal = saturate(lerp(normal1, normal2, uv.y));

    float4 brush1 = lerp(patch[0].BrushColor, patch[1].BrushColor, uv.x);
    float4 brush2 = lerp(patch[2].BrushColor, patch[3].BrushColor, uv.x);
    float4 brush = lerp(brush1, brush2, uv.y);
    output.BrushColor = brush;

    float2 heightUV = output.uv;
    position.y = map.SampleLevel(samp, heightUV, 0).r * _heightRatio;

    output.uv *= UvAmount;
    

    output.position = mul(float4(position.xyz, 1.0f), World);
    output.oPosition = output.position;

    output.position = mul(output.position, ViewProjection);

    output.normal = mul(output.normal, (float3x3) World);

    return output;
}

Texture2D _diffuseTex : register(t0);
Texture2D _specularTex : register(t1);
Texture2D _emissiveTex : register(t2);
Texture2D _normalTex : register(t3);
Texture2D _detailTex : register(t4);

SamplerState wrapSamp : register(s1);

/***************************************************************
Basic Deferred Shading 
****************************************************************/
G_Buffer PackGBuffer(G_Buffer buffer, float3 normal, float3 diffuse, float SpecIntensity, float SpecPower)
{
    G_Buffer Out = buffer;

	// Normalize the specular power
    float SpecPowerNorm = max(0.0001, (SpecPower - g_SpecPowerRange.x) / g_SpecPowerRange.y);

	// Pack all the data into the GBuffer structure
    Out.diffuse = float4(diffuse.rgb, SpecIntensity);
    Out.normal = float4(normal * 0.5f + 0.5f, 1.0f);
    Out.spec = float4(SpecPowerNorm, 0.0f, 0.0f, 1.0f);

    return Out;

}

Texture2D<float4> SplitMap : register(t5);
Texture2D srcTex[4] : register(t6);

float4 CalCuSplat(float4 diffuse, float2 uv)
{
    //TODO 수누꺼로 바꾸기로~

    float4 color = diffuse;

    float4 splatMap = SplitMap.Sample(wrapSamp, uv / UvAmount);
    float splat[4] = { splatMap.r, splatMap.g, splatMap.b, splatMap.a };
    
    for (int i = 0; i < 4; i ++)
    {
        float4 src = srcTex[i].Sample(wrapSamp, uv);
        color += splat[i] * src;
    }

    color /= color.a + 1.0f;

    return color;
}

G_Buffer TerrainToolPS(PixelInput input)
{
    G_Buffer output;

    //output.worldPos = float4(SplitMap.Sample(wrapSamp, input.uv / UvAmount).rgb, 1); //input.oPosition; //  SplitMap.Sample(wrapSamp, input.uv / UvAmount);
    output.diffuse = _diffuseTex.Sample(wrapSamp, input.uv);
    //==========================TEST
    output.diffuse = CalCuSplat(output.diffuse, input.uv);
    //==============================
    output.diffuse += input.BrushColor;
    output = PackGBuffer(output, input.normal, output.diffuse.rgb, 0.25f, 250.0f);

    output.spec = _specularTex.Sample(_basicSampler, input.uv);

    output.normal = float4(input.normal.xyz, 1.0f);
    output.normal.a = 1.5f;

    if (GridbView >= 0.9f)
    {
        float2 pos = input.oPosition.xz ;


        float2 floorPos = floor(pos) + 1;

        float2 grid = input.oPosition.xz;
        float2 range = abs(frac(grid - 0.5f) - 0.5f);
        float2 speed = fwidth(grid);
        
        float2 pixel = range / speed;

        float weight = saturate(min(pixel.x, pixel.y) - GridThickness);

        output.diffuse = lerp(float4(1, 1, 1, 1), output.diffuse, weight);
    }
    output.diffuse.a = 1.0f;

    return output;

}