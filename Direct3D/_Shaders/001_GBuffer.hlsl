#include "000_Header.hlsl"


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

struct BasicPixelInput
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

BasicPixelInput BasicDeferredVS(VertexTextureNormal input)
{
    BasicPixelInput output;

    output.position = output.worldPos = mul(input.position, _world);
    output.position = mul(output.position, _viewProjection);

    output.normal = normalize(mul(input.normal, (float3x3) _world));

    output.uv = input.uv;

    return output;
}


G_Buffer BasicDeferredPS(BasicPixelInput input)
{
    G_Buffer output;

    output.worldPos = input.worldPos;
    output.diffuse = _diffuseTex.Sample(_basicSampler, input.uv);
    output = PackGBuffer(output, input.normal, output.diffuse.rgb, 0.25f, 250.0f);

    output.spec = _specularTex.Sample(_basicSampler, input.uv);

    return output;

}



/***************************************************************
Color Deferred Shading 
****************************************************************/

struct ColorNormalPixelInput
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};



ColorNormalPixelInput ColorDeferredVS(VertexColorNormal input)
{
    ColorNormalPixelInput output;

    output.position = output.worldPos = mul(input.position, _world);
    output.position = mul(output.position, _viewProjection);

    output.normal = mul(input.normal, (float3x3) _world);

    output.color = input.color;

    return output;
}

G_Buffer ColorDeferredPS(ColorNormalPixelInput input)
{
    G_Buffer output;

    output.worldPos = input.worldPos;
    output.diffuse = input.color;
    output = PackGBuffer(output, input.normal, output.diffuse.rgb, 1.0f, 250.0f);

    return output;
}

/***************************************************************
Gizmo Deferred Shading 
****************************************************************/

struct ColorPixelInput
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION0;
    float4 color : COLOR0;
};


ColorPixelInput GizmoDeferredVS(VertexColorNormal input)
{
    ColorPixelInput output;

    output.position = output.worldPos = mul(input.position, _world);
    output.position = mul(output.position, _viewProjection);

    output.color = input.color;

    return output;
}

G_Buffer GizmoDeferredPS(ColorPixelInput input)
{
    G_Buffer output;

    output.worldPos = input.worldPos;
    output.diffuse = input.color;
    output.spec = float4(1, 1, 1, 2);
    output.normal = float4(0, 0, 0, 1);

    return output;
}