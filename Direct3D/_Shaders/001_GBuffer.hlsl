#include "000_Header.hlsl"


/***************************************************************
Basic Deferred Shading 
****************************************************************/

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
    output.spec = _specularTex.Sample(_basicSampler, input.uv);
    output.normal = float4(normalPacking(input.normal), 1.0f);

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
    output.spec = float4(1, 1, 1, 1);
    output.normal = float4(normalPacking(input.normal), 1);

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