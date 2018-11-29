#include "Particle_Header.hlsl"


cbuffer ViewProjectionBuffer : register(b0)
{
    matrix View;
    matrix Projection;
    matrix ViewProjection;
    matrix InvView;
    matrix InvProjection;
    matrix InvViewProjection;
}

cbuffer CS_GenerateData : register(b1)
{
    float Time;
    float3 StartDirection;

    float LifeTime;
    float3 Gravity;

    float3 Force; //매 프레임 파티클 방향으로 적용되는 힘.
    int ShapeType;


    float3 Position;
    int EmitCount;

    float2 StartSize;
    float2 EndSize;

    float VelocityMax;
    float VelocityMin;
    int RandomRotation;
    float Padding;

    //ShapeData Shape;

    matrix EmitterRotation;
    float4 EmitterScale;

    float Saturation;
    float Value;
    float2 Padding2;
};

cbuffer CS_UpdateData : register(b2)
{
    //------------
    float Delta;
    uint Padding_Update;
    uint RandomSeed;
    float CameraSpeed;
    //------------
    float3 StretchedScale;
    uint RenderMode;
    //------------
};

cbuffer ParticleAnimation : register(b3)
{
    int2 MaxIndex;
    float2 LoopCount; // bLoop ,count

    float3 Fps;
    float ParticleAnimation_Padding;

};

RWStructuredBuffer<ParticleData> consumeBuffer : register(u0);
RWStructuredBuffer<ParticleData> appendBuffer : register(u1);

RWStructuredBuffer<uint> consumeCounter : register(u2);
RWStructuredBuffer<uint> appendCounter : register(u3);

float GetRadianFromDirection(float3 direction)
{
    return atan2(direction.x, direction.z);
}

[numthreads(32, 32, 1)]
void Update(uint3 id : SV_DispatchThreadID)//SV_DispatchThreadID SV_GroupIndex
{
    uint numParticles = consumeCounter[0];
    if (id.x == 0 && id.y == 0)
        appendCounter[0] = 0;
    DeviceMemoryBarrierWithGroupSync();

    if (id.x + id.y * 32 >= numParticles)
        return;
    if (consumeCounter[0] <= 0)
        return;
 
    uint consumeIndex = 0;
    InterlockedAdd(consumeCounter[0], -1, consumeIndex);
    ParticleData particle = consumeBuffer[consumeIndex - 1];

    //LifeTime 체크
    particle.RemainTime -= Delta;
    if (particle.RemainTime <= 0.00f)
    {
        particle.Size = float2(0, 0);
        return;
    }

    //애니메이션 프레임 갱신
    if(particle.bLoop == 1)
    {
        particle.FrameTimer += Delta;
        float unit = 1 / particle.Fps;

        if (particle.FrameTimer >= unit)
        {
            particle.FrameTimer = 0.0f;
            particle.NowFrame = ++particle.NowFrame % (int) (MaxIndex.x * MaxIndex.y);

        }
    }
    else
    {
        if (particle.LoopCount < 0)
            return;

        particle.FrameTimer += Delta;
        float unit = 1 / particle.Fps;

        if (particle.FrameTimer >= unit)
        {
            particle.FrameTimer = 0.0f;
            particle.NowFrame = ++particle.NowFrame % (int) (MaxIndex.x * MaxIndex.y);
            particle.LoopCount -= 1;

        }
    }

    //라이프 타임에 따른 보간처리
    float ratio = 1.0f - (particle.LifeTime / particle.RemainTime);
    float pointUnit, interpolate, startIndex, endIndex;

    //스피드
    //if (CurveCounts.x > 0)
    //{
    //    pointUnit = 1.0f / (CurveCounts.x);
    //    interpolate = frac(ratio / pointUnit);
    //    startIndex = (int) (ratio / pointUnit);
    //    endIndex = min(startIndex + 1, CurveCounts.x - 1);

    //    particle.SimulationForce = lerp(SpeedCurvePoint[startIndex],
    //    SpeedCurvePoint[endIndex], interpolate);
    //}
    //사이즈
    //if (CurveCounts.y > 0)
    //{
    //    pointUnit = 1.0f / (CurveCounts.y);
    //    interpolate = frac(ratio / pointUnit);
    //    startIndex = (int) (ratio / pointUnit);
    //    endIndex = min(startIndex + 1, CurveCounts.y - 1);

    //    particle.Size = lerp(SizeCurvePoint[startIndex],
    //    SizeCurvePoint[endIndex], interpolate);
    //}
    //색상
    //if (CurveCounts.z > 0)
    //{
    //    pointUnit = 1.0f / (CurveCounts.z);
    //    interpolate = frac(ratio / pointUnit);
    //    startIndex = (int) (ratio / pointUnit);
    //    endIndex = min(startIndex + 1, CurveCounts.z - 1);

    //    particle.Color = lerp(ColorCurvePoint[startIndex],
    //    ColorCurvePoint[endIndex], interpolate);
    //}

    particle.Speed += (particle.Force + particle.Gravity) * Delta;
    particle.Position += particle.Speed * Delta;


    //빌보드 적용
    float3 forward, up, right, halfWidth, halfHeight;
    //일반 빌보드
    if (RenderMode == 0)
    {
        forward = normalize(InvView._41_42_43 - particle.Position);
        up = InvView._21_22_23;
        right = normalize(cross(forward, up));
        halfWidth = particle.Size.x * 0.5f;
        halfHeight = particle.Size.y * 0.5f;
    }
    //Stretched 빌보드
    else if (RenderMode == 1)
    {
        //forward = normalize(_viewInverse._41_42_43 - particle.Center);
        //up = particle.Direction;
        //right = normalize(cross(forward, up));
        //halfWidth = particle.Size.x * 0.5f;
        //halfHeight = particle.Size.y * 0.5f;

        //halfHeight *= CameraSpeed * StretchedScale.x +
        //particle.Speed * StretchedScale.y + particle.Size.x * StretchedScale.z;
    }
    //바닥 빌보드
    else if (RenderMode == 2)
    {
        forward = float3(0, 1, 0);
        up = float3(0, 0, -1);
        right = float3(1, 0, 0);
        halfWidth = particle.Size.x * 0.5f;
        halfHeight = particle.Size.y * 0.5f;
    }

    //회전 변환
    if (particle.Rotation > 0)
    {
        float3x3 matRotation;
        GetMatrixRotation(matRotation, forward, particle.Rotation);

        right = mul(right, matRotation);
        up = mul(up, matRotation);
    }

    ///*
    //    0 --- 2
    //    |  /  |
    //    1 --- 3
    //*/
    particle.QuadPositions[0] = particle.Position - right * halfWidth + up * halfHeight;
    particle.QuadPositions[1] = particle.Position - right * halfWidth - up * halfHeight;
    particle.QuadPositions[2] = particle.Position + right * halfWidth + up * halfHeight;
    particle.QuadPositions[3] = particle.Position + right * halfWidth - up * halfHeight;

    //갱신된 파티클을 Apeend 버퍼에 추가한다.
    uint currentCount = 0;
    InterlockedAdd(appendCounter[0], 1, currentCount);

    currentCount = currentCount % 1000;
    appendBuffer[currentCount] = particle;

}

