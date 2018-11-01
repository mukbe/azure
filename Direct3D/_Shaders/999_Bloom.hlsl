Texture2D<float4> DeferredTexture : register(t0);
StructuredBuffer<float> AvgLum : register(t1);
Texture2D<float4> BloomTex : register(t2);

SamplerState PointSampler : register(s3);
SamplerState LinearSampler : register(s4);

static const float2 arrBasePos[4] =
{
    float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

static const float2 arrUV[4] =
{
    float2(0.0, 0.0),
	float2(1.0, 0.0),
	float2(0.0, 1.0),
	float2(1.0, 1.0),
};

struct VS_OUTPUT
{
    float4 Position : SV_Position; // vertex position 
    float2 UV : TEXCOORD0;
};

cbuffer FinalPassConstants : register(b1)
{
    float MiddleGrey;
    float LumWhiteSqr;
    float BloomScale;
    int HDRActive;
    int bloomActive;
    float3 padding;
}

static const float3 LUM_FACTOR = float3(0.299, 0.587, 0.114);

float3 ToneMapping(float3 HDRColor)
{
    float LScale = dot(HDRColor, LUM_FACTOR);
    LScale *= MiddleGrey / AvgLum[0];
    LScale = (LScale + LScale * LScale / LumWhiteSqr) / (1.0 + LScale);
    return HDRColor * LScale; 
}

VS_OUTPUT VS(uint VertexID : SV_VertexID)
{
    VS_OUTPUT Output;

    Output.Position = float4(arrBasePos[VertexID].xy, 0.0, 1.0);
    Output.UV = arrUV[VertexID].xy;
    
    return Output;
}


float4 PS(VS_OUTPUT In) : SV_TARGET
{
    float3 color = DeferredTexture.Sample(PointSampler, In.UV.xy).xyz;

    if (HDRActive < 1)
        return float4(color, 1);

    if (bloomActive > 0.1f)
        color += BloomScale * BloomTex.Sample(LinearSampler, In.UV.xy).xyz;

    color = ToneMapping(color);

    return float4(color, 1.0);
}
