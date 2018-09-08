#include "000_Header.hlsl"

Texture2D texArray[5] : register(t0);

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


struct VS_INPUT
{
    float4 position : POSITION0;
    float3 normal : NORMAL0;
    float2 scale : TEXCOORD0;
    int textureID : TEXCOORD1;
};


struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 worldPos : POSITION0;
    float2 uv : TEXCOORD6;
    int textureIndex : TEXCOORD7;
};

VS_INPUT VS(VS_INPUT input)
{
    return input;
}


[maxvertexcount(4)]
void GS(point VS_INPUT gin[1],
            inout TriangleStream<PS_INPUT> triStream)
{
    float3 up;
    float3 look;
    float3 right;

    look = gin[0].normal;
    
    up = float3(0, 1, 0);
    up = normalize(up);
    look = float3(InvView._41, InvView._42, InvView._43);
    right = normalize(cross(up, look));

    float halfWidth = gin[0].scale.x / 2.0f;
    float2 scale = gin[0].scale;
	
    float3 position = gin[0].position;
    float4 v[4];
    float2 uv[4];
    v[0] = float4(position + halfWidth * right, 1.0f);
    uv[0] = float2(1.f, 1.f);
    v[1] = float4(position + halfWidth * right + scale.y * up, 1.0f);
    uv[1] = float2(1.f, 0.f);
    v[2] = float4(position - halfWidth * right, 1.0f);
    uv[2] = float2(0.f, 1.f);
    v[3] = float4(position - halfWidth * right + scale.y * up, 1.0f);
    uv[3] = float2(0.f, 0.f);

	[unroll]
    for (int i = 0; i < 4; ++i)
    {
        PS_INPUT gout;
        gout.position = mul(v[i], ViewProjection);
        gout.worldPos = v[i];
        gout.uv = uv[i];
        gout.textureIndex = gin[0].textureID;

        triStream.Append(gout);
    }
}

G_Buffer PS(PS_INPUT input)
{
    G_Buffer output;

    float4 texColor = texArray[input.textureIndex].Sample(_basicSampler, input.uv);
    if (texColor.a < 0.1f)
        discard;

    output.diffuse = texColor.rgb;

    return output;

}