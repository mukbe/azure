
cbuffer VS_ViewProjection : register(b0)
{
    matrix _vsView;
    matrix _vsProjection;
    matrix _vsViewProjection;
}

cbuffer VS_WorldBuffer : register(b1)
{
    matrix _vsWorld;
}

struct VS_INPUT
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    output.position = mul(input.position, _vsWorld);
    output.position = mul(output.position, _vsView);
    output.position = mul(output.position, _vsProjection);

    output.color = input.color;

    return output;
}

float4 PS(PS_INPUT input ) : SV_Target
{
    return input.color;
}






