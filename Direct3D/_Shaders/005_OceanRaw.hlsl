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
    float3 normal : NORMAL0;
    float3 color : TEXCOORD0;
};

G_Buffer PackGBuffer(G_Buffer buffer, float3 normal, float3 diffuse, float3 specColor,
    float SpecIntensity, float SpecPower, float renderType)
{
    G_Buffer Out = buffer;

	// Normalize the specular power
    float SpecPowerNorm = max(0.0001, (SpecPower - g_SpecPowerRange.x) / g_SpecPowerRange.y);

	// Pack all the data into the GBuffer structure
    Out.normal = float4(normal * 0.5f + 0.5f, renderType);
    Out.diffuse = float4(diffuse.rgb, SpecIntensity);
    Out.spec = float4(specColor, SpecPowerNorm);

    return Out;

}

Texture2D _lookUp : register(t6);

float Fresnel(float3 V, float3 N)
{
    float costhetai = abs(dot(V, N));
    float3 texelUV = GetTexelUV(_lookUp, float2(costhetai, 0.0f));
    return _lookUp.Load(texelUV).a;
}


PixelInput InstanceVS(VertexInput input)
{
    PixelInput output;

    float3 worldPos = input.position.xyz + float3(input.offset.x, 0, input.offset.y);
   
    float3 sunDirection = normalize(float3(-1, -1, 0));

    float3 V = normalize(GetCameraPosition() - worldPos.xyz);
    float3 N = normalize(input.normal);
	
    float fresnel = Fresnel(V, N);
    float4 specColor = lerp(DiffuseColor, AmbientColor, fresnel);

    float diffuseFactor = saturate(dot(input.normal, -sunDirection));
    float3 diffuseColor = DiffuseColor * AmbientColor * diffuseFactor;

    output.position = mul(float4(worldPos,input.position.w), ViewProjection);
    output.normal = input.normal;
    output.color = diffuseColor + specColor.rgb;

    return output;
}


G_Buffer InstancePS(PixelInput input)
{
    G_Buffer output;

    output.normal = float4(input.normal, 1.5f);
    output.diffuse = float4(input.color, 1.0f);
    output.spec = float4(0, 0, 0, 0);

    return output;
}
