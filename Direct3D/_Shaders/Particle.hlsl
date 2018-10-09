#include "000_Header.hlsl"

struct ParticleData
{
    bool isActive; // 
    float3 position; // 
    float3 velocity; // 
    float4 color; // 
    float duration; // 
    float scale; // 
};

struct v2f
{
    float2 uv : TEXCOORD0;
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float scale : TEXCOORD1;
};

StructuredBuffer<ParticleData> _Particles : register(t8);
//TODO 
//인덱스버퍼를 컴퓨트에서 계산해 가져온다 
//그러고 드로우를 콜 할때 인덱스의 크기많큼 해주고 id값으로 파티클의 id를 찾는다
			
struct Vertex
{
    uint id : SV_VertexID;
};

v2f ParticleVS(Vertex input)
{
    v2f o;
    
    uint id = input.id;
    o.pos = float4(_Particles[id].position, 1);
    o.uv = float2(0, 0);
    o.col = _Particles[id].color;
    o.scale = _Particles[id].isActive ? _Particles[id].scale : 0;

    return o;
}
			

[maxvertexcount(4)]
void ParticleGS(point v2f input[1], inout TriangleStream<v2f> outStream)
{
    v2f o;

    float4 pos = input[0].pos;
    float4 col = input[0].col;
    o.scale = 0;

    for (int x = 0; x < 2; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            float2 uv = float2(x, y);
            o.uv = uv;

            float3 planeNormal = input[0].pos.xyz - GetCameraPosition();
            planeNormal.y = 0.f;
            planeNormal = normalize(planeNormal);

            float3 upVector = float3(0, 1, 0) * input[0].scale;
            float3 rightVector = normalize(cross(planeNormal, upVector)) * input[0].scale;
            

            float2 rate = float2(1, 1) - uv * 2;
            o.pos = pos + float4(rate.y * upVector * 0.5f + rate.x * rightVector * 0.5f, 0);
            o.pos = mul(o.pos,ViewProjection);

            o.col = col;

            outStream.Append(o);
        }
    }

    outStream.RestartStrip();
}


G_Buffer PackGBuffer(G_Buffer buffer, float3 normal, float3 diffuse, float SpecIntensity, float SpecPower)
{
    G_Buffer Out = buffer;

	// Normalize the specular power
    float SpecPowerNorm = max(0.0001, (SpecPower - g_SpecPowerRange.x) / g_SpecPowerRange.y);

	// Pack all the data into the GBuffer structure
    Out.diffuse = float4(diffuse.rgb, SpecIntensity);
    Out.normal = float4(normal * 0.5f + 0.5f, 1.0f);
    Out.spec = float4(SpecPowerNorm, 0.0f, 0.0f, 1.0f);

    return Out;

}

G_Buffer ParticlePS(v2f input)
{
    G_Buffer output;

    //output.worldPos = float4(SplitMap.Sample(wrapSamp, input.uv / UvAmount).rgb, 1); //input.oPosition; //  SplitMap.Sample(wrapSamp, input.uv / UvAmount);
    output.diffuse = input.col * 10.f;

    output = PackGBuffer(output, float3(0, 1, 0), output.diffuse.rgb, 0.25f, 250.0f);

    output.spec = float4(0, 0, 0, 0);

    output.normal = float4(float3(0, 1, 0), 1.0f);
    output.normal.a = 1.5f;

    output.diffuse.a = 1.5f;

    return output;

}