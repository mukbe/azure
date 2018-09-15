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
    return _lookUp.Sample(_basicSampler, float2(costhetai, 0.0)).a;

     //uint width, height;
    //_lookUp.GetDimensions(width, height);
    //float3 texCoord = float3(costhetai * width, 0.0f, 0.0f);
    //return _lookUp.Load(texCoord).a;
    //return tex2D(_lookUp, float2(costhetai, 0.0)).a;
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
    float3 N = normalize(input.normal);
	
   float fresnel = Fresnel(V, N);
   float4 specColor = lerp(DiffuseColor, AmbientColor, fresnel);
   float3 sunDirection = normalize(float3(-1, -1, 0));
   
   float diffuseFactor = saturate(dot(input.normal, - sunDirection));
    float4 diffuse = DiffuseColor * AmbientColor * diffuseFactor;

    output.normal = float4(input.normal, 1);
    output.diffuse = specColor;
    output.spec = float4(0, 0, 0, 0);

    return output;
}