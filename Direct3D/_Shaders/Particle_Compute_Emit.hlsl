#include "Particle_Header.hlsl"

inline float rnd(float2 p)
{
    return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

inline float3 rnd3(float2 p)
{
    return 2.0 * (float3(rnd(p * 1), rnd(p * 2), rnd(p * 3)) - 0.5);
}

// ����(Hue), ä��(Saturation), ��(Value)  HSV ������ RGB��������
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

    float3 Force; //�� ������ ��ƼŬ �������� ����Ǵ� ��.
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


    //Shape �����͸� Ȯ���ؼ�, Position�� DIrection�� �����Ѵ�.
    switch (ShapeType)
    {
        case SHAPE_CIRCLE:
        {
            //    float randomAngle = Random(RandomSeed + id.x + 5,
            //0, Shape.CircleAngle); //Radian

            //    particle.Direction = float3(sin(randomAngle), 0, cos(randomAngle));
            //    particle.Direction = mul(particle.Direction, (float3x3) EmitterRotation);

            //    float distanceScale = Random(RandomSeed + id.x + 6, Shape.RadiusRange.x, Shape.RadiusRange.y);
            //    particle.Position = GeneratePosition + particle.Direction * EmitterScale.x * 0.5f * distanceScale;
        }
        break;

        case SHAPE_SPHERE:
        {
                //particle.Direction = GetRandomVector3(RandomSeed + id.x + 5);
            
                //float distanceScale = Random(RandomSeed + id.x + 6, Shape.RadiusRange.x, Shape.RadiusRange.y);
                //particle.Position = GeneratePosition + particle.Direction * EmitterScale.x * 0.5f * distanceScale;
        }
        break;

        case SHAPE_BOX:
        {           
                //float3 distanceScale = float3(
                //    Random(RandomSeed + id.x + 5, -0.5f, 0.5f),
                //    Random(RandomSeed + id.x + 6, -0.5f, 0.5f),
                //    Random(RandomSeed + id.x + 7, -0.5f, 0.5f));

                float3 emitterUp = EmitterRotation._21_22_23;
                float3 emitterRight = EmitterRotation._11_12_13;
                float3 emitterForward = StartDirection;

            //    particle.Position = Position + EmitterScale.x * distanceScale.x * emitterRight
            //+ EmitterScale.y * distanceScale.y * emitterUp
            //+ EmitterScale.z * distanceScale.z * emitterForward;

                particle.Direction = emitterForward;
        }
        break;

        default:
            particle.Position = Position;
            particle.Direction = StartDirection;
        break;
    }
    //if (Shape.Inverse > 0)
    //    particle.Direction = -particle.Direction;

    //������
    if (StartSize.x == EndSize.x && StartSize.y == EndSize.y)
        particle.Size = float2(StartSize.x, StartSize.y);
    else
    {
        float2 randSize = GetRandomVector2((uint)(rnd(Time + id.x + 1.58f) * 50000), StartSize, EndSize, false);
        particle.Size = randSize;
    }

    //���ǵ�
    uint no = id.x;
    float2 seed = float2(no + Time, no + 1.583 + Time);
    float speed = clamp(rnd(seed) * VelocityMax, VelocityMin, VelocityMax);
    
    particle.Speed = normalize(StartDirection) * speed;
    particle.Speed = normalize(rnd3(seed)) * speed;

    //Life Time
    //if (LifeTime.x == LifeTime.y)
    //    particle.LifeTime = float2(LifeTime.x, LifeTime.x);
    //else
    //    particle.LifeTime = Random(RandomSeed + id.x + 2, LifeTime.x, LifeTime.y);
    particle.RemainTime = particle.LifeTime = LifeTime;

    //����
    float h = rnd(seed + 5);
    particle.Color = float4(hsv_to_rgb(float3(h, Saturation, Value)), 1);

    //���ӵ�
    particle.Gravity = Gravity;
    particle.Force = Force;

    //ȸ��
    //particle.Rotation = 0;
    //if (RandomRotation > 0)
    //{
    //    particle.Rotation = Random(RandomSeed + id.x + 12, 0, 2 * PI);
    //}

    //�ؽ��� �ִϸ��̼�
    if (MaxIndex.x >= 0)        //0�̸� �ؽ�ó �ϳ� �ִ°�
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

    //TODO currentCount + id.x �� Ȯ�� �� ���ϴ°��� �ȴ��ϴ°� �´°� ������
    currentCount = currentCount % 1000;
    generateBuffer[currentCount] = particle;

}
