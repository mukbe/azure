#include "Particle_Header.hlsl"

cbuffer PS_TextureAnimation : register(b3)
{
    float2 TextureTile;
    float2 StartFrameRange;

    int FrameOverTime;
    int Cycles;
    float2 TAPadding;
}

RWStructuredBuffer<uint> counterBuffer : register(u0);
RWStructuredBuffer<uint> counterBuffer2 : register(u1);

RWStructuredBuffer<ParticleData> particleData : register(u2);
RWStructuredBuffer<ParticleData> particleData2 : register(u3);

RWStructuredBuffer<uint> indirectBuffer : register(u4);


[numthreads(1, 1, 1)]
void CopyIndirect(uint3 id : SV_DispatchThreadID)
{
    indirectBuffer[0] = counterBuffer[0];
}

[numthreads(32, 32, 1)]
void ResetCounter(uint3 id : SV_DispatchThreadID)
{
    counterBuffer[0] = 0;
    counterBuffer2[0] = 0;
    DeviceMemoryBarrierWithGroupSync();

    uint index = id.x + id.y * 32;
    if (index >= 1024)
        return;

    particleData[index].LifeTime = -0.5f;
    particleData2[index].LifeTime = -0.5f;
}