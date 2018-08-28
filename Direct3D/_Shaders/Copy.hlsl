//StructuredBuffer<float> heightMap : register(t0);
//RWStructuredBuffer<float> RWheightMap : register(u0);

//Texture2D heightSrc : register(t1);

//[numthreads(16, 16, 1)]
//void heightMapSet(uint3 dtid : SV_DispatchThreadID)
//{
//    uint2 temp;
//    heightSrc.GetDimensions(temp.x, temp.y);

//    uint2 index = clamp(dtid.xy, uint2(0, 0), temp.xy - 1);

//    RWheightMap[index.x + index.y * temp.x] = float4(0, 0, 0, 1);
    

//    float4 color = heightSrc[index];
//    RWheightMap[index.x + index.y * temp.x] = color.r * 7.5f;

//}

Texture2D<float4> resource : register(t0);
RWTexture2D<float4> output : register(u0);

[numthreads(16, 16, 1)]
void CopyTexture(uint3 dtid : SV_DispatchThreadID)
{
    uint2 temp;
    resource.GetDimensions(temp.x, temp.y);

    uint2 index = clamp(dtid.xy, uint2(0, 0), temp.xy - 1);

    float4 color = resource[index];
    output[dtid.xy] = color;
}

