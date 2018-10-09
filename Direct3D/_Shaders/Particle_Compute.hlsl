#define THREAD_NUM_X 16

struct ParticleData
{
    bool isActive; 
    float3 position; 
    float3 velocity;  
    float4 color; 
    float duration;  
    float scale; 
};


inline float rnd(float2 p)
{
    return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

inline float3 rnd3(float2 p)
{
    return 2.0 * (float3(rnd(p * 1), rnd(p * 2), rnd(p * 3)) - 0.5);
}

float3 hsv_to_rgb(float3 HSV)
{
    float4 k = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(HSV.xxx + k.xyz) * 6.0 - k.www);
    return HSV.z * lerp(k.xxx, clamp(p - k.xxx, 0.0, 1.0), HSV.y);
}

RWStructuredBuffer<ParticleData> _Particles : register(u0);
AppendStructuredBuffer<uint> _DeadList : register(u1);
ConsumeStructuredBuffer<uint> _ParticlePool : register(u2);

cbuffer ParticleBuffer : register(b0)
{
    float3 _EmitPosition;
    float _VelocityMax;

    float _DT;
    float _Time;
    float _LifeTime;
    float _ScaleMin;

    float _ScaleMax;
    float _Gravity;
    float _Sai;
    float _Val;
}

[numthreads(THREAD_NUM_X, 1, 1)]
void Init(uint3 id : SV_DispatchThreadID)
{
    uint no = id.x;
    
    _Particles[no].isActive = false;
    _DeadList.Append(no);
}

[numthreads(THREAD_NUM_X, 1, 1)]
void Emit(uint3 id : SV_DispatchThreadID)
{
    uint no = _ParticlePool.Consume();

    float2 seed = float2(no + _Time, no + 1.583 + _Time);
    float speed = rnd(seed) * _VelocityMax;
    float scale = (rnd(seed + 3) - 0.5) * 2.0 * (_ScaleMax - _ScaleMin) + _ScaleMin;
    float h = rnd(seed + 5); 
    
    _Particles[no].position = _EmitPosition;
    _Particles[no].velocity = (rnd3(seed + 3.15)) * speed;
    _Particles[no].color = float4(hsv_to_rgb(float3(h, _Sai, _Val)), 1);
    _Particles[no].duration = _LifeTime;
    _Particles[no].scale = scale;
    _Particles[no].isActive = true;
    

}

[numthreads(THREAD_NUM_X, 1, 1)]
void Update(uint3 id : SV_DispatchThreadID)//SV_DispatchThreadID SV_GroupIndex
{
    
    uint no = id.x;

    if (_Particles[no].isActive)
    {
        _Particles[no].velocity.y -= _Gravity * _DT;
        _Particles[no].position += _Particles[no].velocity* _DT;
        _Particles[no].duration -= _DT;
        _Particles[no].color.a = max(_Particles[no].duration / _LifeTime, 0);
        if (_Particles[no].duration <= 0)
        {
            _Particles[no].isActive = false;
            _DeadList.Append(no);
        }
    }

}
