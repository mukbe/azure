
Texture2D<float4> srv : register(t0);

RWTexture2D<float4> uav : register(u0);

[numthreads(16, 36, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    float depth = srv[DTid.xy].r;
    float4 color = float4(1.0 - depth, 1.0 - depth, 1.0 - depth, 1.0);
    color *= 100.0f;
    uav[DTid.xy] = color;

}