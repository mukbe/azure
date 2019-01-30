#include "000_Header.hlsl"
#include "Particle_Header.hlsl"


cbuffer ParticleAnimation : register(b3)
{
    int2 MaxIndex;
    float2 LoopCount; // bLoop ,count

    float3 Fps;
    float ParticleAnimation_Padding;

};


struct VS_OUTPUT
{
    float4 QuadPositions[4] : POSITION0;
    float4 Color : COLOR0;
    int NowFrame : FRAME0;
};

StructuredBuffer<ParticleData> Particles : register(t0);
VS_OUTPUT VS(uint input : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        output.QuadPositions[i] = float4(Particles[input].QuadPositions[i], 1.0f);
    }
    output.Color = Particles[input].Color;
    output.NowFrame = Particles[input].NowFrame;

    return output;
}

////////////////////////////////////////////////////////////////////////////////

struct GS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
    int nowFrame : FRAME0;
};

static const float2 Texcoords[4] =
{
    float2(0, 0),
    float2(0, 1),
    float2(1, 0),
    float2(1, 1)
};

[maxvertexcount(4)]
void GS(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> outputStream)
{
    GS_OUTPUT output = (GS_OUTPUT)0;

    for (int i = 0; i < 4; ++i)
    {

        output.position = mul(input[0].QuadPositions[i], ViewProjection);
        output.uv = Texcoords[i];
        output.color = input[0].Color;
        output.nowFrame = input[0].NowFrame;

        outputStream.Append(output);
    }
    outputStream.RestartStrip();
}

////////////////////////////////////////////////////////////////////////////////
Texture2D particleTexture : register(t0);
SamplerState samp : register(s0);

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



float4 PS(GS_OUTPUT input)  : SV_Target0
{
    float4 output;

    if(MaxIndex.x * MaxIndex.y <= 0)
    {
        output = input.color * 5.f;
    }
    else
    {
        float2 uvUnit = float2(1.0f, 1.0f) / MaxIndex;
        uint2 frame = uint2(0, 0);
        frame.y = (uint) (input.nowFrame / MaxIndex.x);
        frame.x = (uint) input.nowFrame - frame.y * (uint) MaxIndex.x;

        float2 uv = input.uv * uvUnit;

        uv += uvUnit * (float2) frame;

        output = particleTexture.Sample(samp, uv);

    }

    if (output.a < 0.1f)
        discard;


    return output;

    //output.diffuse = input.color * 5.f;

    //output = PackGBuffer(output, float3(0, 1, 0), output.diffuse.rgb, 0.25f, 250.0f);

    //output.spec = float4(0, 0, 0, 0);

    //output.normal = float4(float3(0, 1, 0), 1.0f);
    //output.normal.a = 1.5f;

}

