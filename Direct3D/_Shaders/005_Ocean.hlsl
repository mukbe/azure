#include "000_Header.hlsl"

struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;

    float2 offset : TEXCOORD1;
};

struct PixelInput
{
    float4 position : SV_POSITION0;
    float3 worldPos : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

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

Texture2D _lookUp : register(t6);

float Fresnel(float3 V, float3 N)
{
    float costhetai = abs(dot(V, N));
    //return tex2D(_lookUp, float2(costhetai, 0.0)).a;
    return _lookUp.Sample(_basicSampler, float2(costhetai, 0.0)).a;

}


PixelInput InstanceVS(VertexInput input)
{
    PixelInput output;
    output.worldPos = input.position.xyz + float3(input.offset.x, 0, input.offset.y);
    output.position = mul(input.position + float4(input.offset.x, 0, input.offset.y, 0), ViewProjection);
    
    output.normal = input.normal;
    output.uv = input.uv;


    return output;
}

G_Buffer InstancePS(PixelInput input)
{
    G_Buffer output;

    float3 V = normalize(GetCameraPosition() - input.worldPos);
    float3 N = input.normal;
		
    float fresnel = Fresnel(V, N);
    float4 specColor = lerp(DiffuseColor, SunColor, fresnel);
    float3 sunDirection = normalize(float3(1, -1, 0));
    
    float diffuseFactor = saturate(dot(input.normal, - sunDirection));
    float4 diffuse = DiffuseColor * SunColor * diffuseFactor;

    output.worldPos = float4(input.worldPos, 1.0f);
    output.diffuse = diffuse + specColor;
    output = PackGBuffer(output, input.normal, output.diffuse.rgb, 0.25f, 250.0f);

    return output;
}