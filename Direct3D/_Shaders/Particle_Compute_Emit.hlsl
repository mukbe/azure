#include "Particle_Header.hlsl"

inline float rnd(float2 p)
{
    return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

inline float3 rnd3(float2 p)
{
    return 2.0 * (float3(rnd(p * 1), rnd(p * 2), rnd(p * 3)) - 0.5);
}

// 색상(Hue), 채도(Saturation), 명도(Value)  HSV 공간을 RGB공간으로
float3 hsv_to_rgb(float3 HSV)
{
    float4 k = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(HSV.xxx + k.xyz) * 6.0 - k.www);
    return HSV.z * lerp(k.xxx, clamp(p - k.xxx, 0.0, 1.0), HSV.y);
}

cbuffer CS_GenerateData : register(b0)
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
    int UseRandom;

    //ShapeData Shape;

    matrix EmitterRotation;
    float4 EmitterScale;

    float Saturation;
    float Value;
    float Alpha;
    float Color;

    ShapeInfo ShapeData;


};

cbuffer ParticleAnimation : register(b3)
{
    int2 MaxIndex;
    float2 LoopCount; // bLoop ,count

    float3 Fps;
    float ParticleAnimation_Padding;

};
#define SHAPE_NONE 0
#define SHAPE_CIRCLE 1
#define SHAPE_SPHERE 2
#define SHAPE_BOX 3

RWStructuredBuffer<ParticleData> generateBuffer : register(u0);
RWStructuredBuffer<uint> counterBuffer : register(u1);

[numthreads(THREAD_NUM_X, 1, 1)]
void Emit(uint3 id : SV_DispatchThreadID)
{
    //if (id.x >= GenerateCount)
    //    return;

    if (counterBuffer[0] > 1024 - THREAD_NUM_X)
        return;

    ParticleData particle = (ParticleData) 0;

    particle.Position = Position;

    uint no = id.x;
    float2 seed = float2(no + Time, no + 1.583 + Time);

    //Life Time
    particle.RemainTime = particle.LifeTime = LifeTime;


    if(UseRandom == 1)
    {
        //사이즈
        if (StartSize.x == EndSize.x && StartSize.y == EndSize.y)
            particle.Size = float2(StartSize.x, StartSize.y);
        else
        {
            float2 randSize = GetRandomVector2((uint) (rnd(Time + id.x + 1.58f) * 50000), StartSize, EndSize, false);
            particle.Size = randSize;
        }

         //스피드
        float speed = clamp(rnd(seed) * VelocityMax, VelocityMin, VelocityMax);
    
        particle.Speed = normalize(StartDirection) * speed;


         //색상
        float h = rnd(seed + 5);
        particle.Color = float4(hsv_to_rgb(float3(h, Saturation, Value)), Alpha);

    }
    else    //랜덤이 아닐 때
    {
        //사이즈
        particle.Size = float2(StartSize.x, StartSize.y);

        //스피드
        uint no = id.x;
        float2 seed = float2(no + Time, no + 1.583 + Time);
        float speed = clamp(rnd(seed) * VelocityMax, VelocityMax, VelocityMax);
    
        particle.Speed = normalize(StartDirection) * speed;


        //색상
        particle.Color = float4(hsv_to_rgb(float3(Color, Saturation, Value)), Alpha);

    }


    int inverse = ShapeData.Inverse ? -1 : 1;

    //Shape 데이터를 확인해서, Position과 DIrection을 결정한다.
    switch (ShapeType)
    {
        case SHAPE_CIRCLE:
        {

            float randomAngle = clamp(rnd(seed) * ShapeData.CircleAngle, 0, ShapeData.CircleAngle);

            particle.Direction = float3(sin(randomAngle), 0, cos(randomAngle)) ;
            particle.Direction = mul(particle.Direction, (float3x3) EmitterRotation);
                
            float dis = Random(id.x + Time + 7, ShapeData.RadiusRange.x, ShapeData.RadiusRange.y); 
            particle.Position = Position + particle.Direction * EmitterScale.x * 0.5f * dis;


            particle.Direction = normalize((particle.Position - Position));
           
            particle.Speed = Random(id.x + Time + 7, VelocityMin, VelocityMax) * particle.Direction * inverse;

        }
        break;

        case SHAPE_SPHERE:
        {
                particle.Direction = GetRandomVector3(id.x + Time + 7);
            
                float dis = Random(id.x + Time + 3, ShapeData.RadiusRange.x, ShapeData.RadiusRange.y);
                particle.Position = Position + particle.Direction * EmitterScale.x * 0.5f * dis;

                particle.Direction = normalize(particle.Position - Position);
                particle.Speed = Random(id.x + Time + 7, VelocityMin, VelocityMax) * particle.Direction * inverse;

        }
        break;

        case SHAPE_BOX:
        {           
                float3 dis = float3(
                    Random(id.x + Time + 3, -0.5f, 0.5f),
                    Random(id.x + Time + 5, -0.5f, 0.5f),
                    Random(id.x + Time + 8, -0.5f, 0.5f));

                float3 emitterUp = EmitterRotation._21_22_23;
                float3 emitterRight = EmitterRotation._11_12_13;
                float3 emitterForward = StartDirection;

                particle.Position = Position + EmitterScale.x * dis.x * emitterRight
            + EmitterScale.y * dis.y * emitterUp
            + EmitterScale.z * dis.z * emitterForward;

                particle.Direction = emitterForward;
                particle.Speed = Random(id.x + Time + 7, VelocityMin, VelocityMax) * particle.Direction * inverse;
            }
        break;

        default:
            particle.Position = Position;
            particle.Direction = StartDirection;
            break;
    }

    //가속도
    particle.Gravity = Gravity;
    particle.Force = Force;

    //회전
    //particle.Rotation = 0;
    //if (RandomRotation > 0)
    //{
    //    particle.Rotation = Random(RandomSeed + id.x + 12, 0, 2 * PI);
    //}

    //텍스쳐 애니메이션
    if (MaxIndex.x >= 0)        //0이면 텍스처 하나 있는것
    {
        particle.NowFrame = 0;

        particle.Fps = Fps.x;

        if (Fps.x < 0.f)
            clamp(rnd(seed) * Fps.z, Fps.y, Fps.z);

        if (LoopCount.x >0.5f)
            particle.bLoop = 1;
        else if (LoopCount.x < 0.5f)
        {
            particle.LoopCount = LoopCount.y;
            particle.bLoop = 0;
        }
    }
    else
        particle.NowFrame = -1;
    
    particle.FrameTimer = 0.0f;


    uint currentCount = 0;
    InterlockedAdd(counterBuffer[0], 1, currentCount);

    //TODO currentCount + id.x 값 확인 왜 더하는건지 안더하는게 맞는거 같은데
    //결국 고치기는 했으나 과연..
    currentCount = currentCount % 1000;
    generateBuffer[currentCount] = particle;

}
