#include "000_Header.hlsl"


struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float3 oPosition : POSITION0;
};


PixelInput VS(VertexColor input)
{
    PixelInput output;
    
    output.position = mul(input.position, World);
    output.position = mul(output.position, ViewProjection);

    output.color = input.color;
    output.oPosition = input.position.xyz;
    return output;
}

/***************************************************************
Basic Deferred Shading 
****************************************************************/
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

G_Buffer PS(PixelInput input)
{
    G_Buffer output;

    output.diffuse = input.color;
    output = PackGBuffer(output, float3(1, 0, 0), output.diffuse.rgb, 0.25f, 250.0f);


    output.normal.a = 1.5f;

    output.normal.a = 1.5f;
    output.diffuse.a = 1.0f;

    return output;

}