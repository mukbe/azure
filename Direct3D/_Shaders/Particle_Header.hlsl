#define THREAD_NUM_X 64

struct ParticleData
{
    float3 Position;
    float4 Color;
    float2 Size;
    float Rotation;

    float3 Direction; //방향
    float3 Speed; //가중치

    float LifeTime; //이놈의 살아있을 시간
    float RemainTime; //남아있는 수명

    float3 Gravity;
    float3 Force;

    int NowFrame;
    float FrameTimer;
    float Fps;
    int LoopCount;
    uint bLoop;

    float3 QuadPositions[4];
};


struct ShapeData
{
    float CircleAngle;
    uint Inverse;
    float2 RadiusRange;
};

// Thomas Wang이 고안한 해쉬 함수
uint Wang_hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

// H. Schechter & R. Bridson이 고안한 해쉬 함수, goo.gl/RXiKaH
uint Hash(uint s)
{
    s ^= 2747636419u;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    return s;
}

// 0 ~ 1
float Random(uint seed)
{
    return float(Hash(seed)) / 4294967295.0; // 2^32-1
}

float Random(uint seed, float start, float end)
{
    float ratio = Random(seed);
    return start * (1 - ratio) + end * ratio;
}

float3 GetRandomVector3(uint seed)
{
    float3 vRand = float3(
    Random(seed + 00000, -1.0f, 1.0f),
    Random(seed + 20000, -1.0f, 1.0f),
    Random(seed + 40000, -1.0f, 1.0f));

    vRand = normalize(vRand);

    return vRand;
}
float3 GetRandomVector3(uint seed, float3 start, float3 end, bool bNormalize = false)
{
    float3 vRand = float3(
    Random(seed + 00000, start.x, end.x),
    Random(seed + 10000, start.y, end.y),
    Random(seed + 20000, start.z, end.z));

    if (bNormalize)
        vRand = normalize(vRand);
    return vRand;
}
float2 GetRandomVector2(uint seed, float2 start, float2 end, bool bNormalize = false)
{
    float2 vRand = float2(
    Random(seed + 25000, start.x, end.x),
    Random(seed + 35000, start.y, end.y));

    if (bNormalize)
        vRand = normalize(vRand);
    return vRand;
}

void GetMatrixRotation(out float3x3 mat, float3 axis, float angle)
{
    float c, s;
    sincos(angle, s, c);

    float t = 1 - c;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    mat = float3x3(
        t * x * x + c, t * x * y - s * z, t * x * z + s * y,
        t * x * y + s * z, t * y * y + c, t * y * z - s * x,
        t * x * z - s * y, t * y * z + s * x, t * z * z + c
    );
}

