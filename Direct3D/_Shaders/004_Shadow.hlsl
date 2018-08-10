#include "000_Header.hlsl"

struct VS_INPUT
{
    float4 position : POSITION0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    output.position = mul(input.position, _vsWorld);
    output.position = mul(output.position, _vsView);
    output.position = mul(output.position, _vsProjection);

    return output;
}

void PS(PS_INPUT input)
{

}