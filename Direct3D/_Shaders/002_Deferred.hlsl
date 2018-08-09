
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
    float3 normal : NORMAL0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

struct PS_OUTPUT
{
    float4 normal : SV_Target0;
    float4 position : SV_Target1;
    float4 diffuse : SV_Target2;
    float4 depth : SV_Target3;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    output.position = mul(input.position, _vsWorld);
    output.worldPos = output.position;
    output.normal = normalize(mul(input.normal, (float3x3)_vsWorld));
    output.position = mul(output.position, _vsViewProjection);

    output.color = input.color;

    return output;
}

PS_OUTPUT PS(PS_INPUT input)
{
    PS_OUTPUT output;
    float depth = input.position.z / input.position.w * 10.0f;

    output.depth = float4(depth, depth, depth, 1.0f);
    output.normal = float4((input.normal * 0.5f + 0.5f),1.0f);
    output.position = input.worldPos;

    output.diffuse = input.color;
   

    return output;

}


