#include "AtmosphericScattering_Header.hlsl"

struct appdata
{
    float4 vertex : POSITION;
};
		

struct v2f
{
    float4 pos : SV_POSITION;
    float4 uv : TEXCOORD0;
    float3 wpos : TEXCOORD1;
};
		               
            
// pass 0 - precompute particle density
struct v2p
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

struct input
{
    float4 vertex : POSITION;
    float2 texcoord : TEXCOORD0;
};

//v2p vertQuad0(input v)
//{
//    v2p o;
//    o.pos = mul(v.vertex, World);
//    o.pos = mul(o.pos, ViewProjection);

//    o.uv = v.texcoord.xy;
//    return o;
//}

//float2 particleDensityLUT(v2p i) : SV_Target
//{
//    float cosAngle = i.uv.x * 2.0 - 1.0;
//    float sinAngle = sqrt(saturate(1 - cosAngle * cosAngle));
//    float startHeight = lerp(0.0, _AtmosphereHeight, i.uv.y);

//    float3 rayStart = float3(0, startHeight, 0);
//    float3 rayDir = float3(sinAngle, cosAngle, 0);
                
//    return PrecomputeParticleDensity(rayStart, rayDir);
//}

[numthreads(16, 16, 1)]
void particleDensityLUT(uint3 id : SV_DispatchThreadID)
{
    //_ParticleDensityLUTCompute
    //720 * 720
    float2 uv = id.xy / 720.f;
    float cosAngle = uv.x * 2.0 - 1.0;
    float sinAngle = sqrt(saturate(1 - cosAngle * cosAngle));
    float startHeight = lerp(0.0, _AtmosphereHeight, uv.y);
    
    float3 rayStart = float3(0, startHeight, 0);
    float3 rayDir = float3(sinAngle, cosAngle, 0);
              
    _ParticleDensityLUTCompute[id.xy] = float4(PrecomputeParticleDensity(rayStart, rayDir), 0.0f, 1.0f);

}

			
// pass 1 - ambient light LUT
v2p vertQuad1(input v)
{
    v2p o;
    o.pos = mul(v.vertex, World);
    o.pos = mul(o.pos, ViewProjection);
    o.uv = v.texcoord.xy;
    return o;
}

float4 fragDir0(v2f i) : SV_Target
{
    float cosAngle = i.uv.x * 1.1 - 0.1; // *2.0 - 1.0;
    float sinAngle = sqrt(saturate(1 - cosAngle * cosAngle));
                    
    float3 lightDir = -normalize(float3(sinAngle, cosAngle, 0));

    return PrecomputeAmbientLight(lightDir);
}

// pass 2 - dir light LUT
v2p vertQuad2(input v)
{
    v2p o;
    o.pos = mul(v.vertex, World);
    o.pos = mul(o.pos, ViewProjection);
    o.uv = v.texcoord.xy;
    return o;
}

float4 fragDir1(v2f i) : SV_Target
{
    float cosAngle = i.uv.x * 1.1 - 0.1; // *2.0 - 1.0;
				
    float sinAngle = sqrt(saturate(1 - cosAngle * cosAngle));
    float3 rayDir = normalize(float3(sinAngle, cosAngle, 0));

    return PrecomputeDirLight(rayDir);
}
			
