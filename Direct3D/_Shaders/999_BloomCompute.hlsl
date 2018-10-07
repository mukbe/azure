//-----------------------------------------------------------------------------------------
// Bloom compute shader
//-----------------------------------------------------------------------------------------

Texture2D<float4> DownScaleSRV : register(t0);
StructuredBuffer<float> AvgLum : register(t1);

RWTexture2D<float4> Bloom : register(u0);

static const float4 Lum_Factor = float4(0.299, 0.587, 0.114, 0);

cbuffer DownScaleBuffer : register(b1)
{
    uint2 Res;
    uint Domain;
    uint GroupSize;
    float Adaptation;
    float fBloomThreshold;
    float2 padding;
}

[numthreads(1024, 1, 1)]
void CSMain(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint2 CurPixel = uint2(dispatchThreadId.x % Res.x, dispatchThreadId.x / Res.x);

    if (CurPixel.y < Res.y)
    {
        float4 color = DownScaleSRV.Load(int3(CurPixel, 0));
        float Lum = dot(color, Lum_Factor);
        float avgLum = AvgLum[0];

        float colorScale = saturate(Lum - avgLum * fBloomThreshold);

        Bloom[CurPixel.xy] = float4(color.rgb * colorScale, 1.0f);
    }
}