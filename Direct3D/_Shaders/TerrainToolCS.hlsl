#include "000_HeaderProp.hlsl"

cbuffer ViewProjectionBuffer : register(b0)
{
    matrix View;
    matrix Projection;
    matrix ViewProjection;
    matrix InvView;
    matrix InvProjection;
    matrix InvViewProjection;
}

cbuffer TerrainTool : register(b1)
{
    float2 MouseScreenPos;
    int BrushStyle;
    float BrushSize;

    float3 PickPos;
    float HeightAmount;

    uint CurrentTexture;
}

RWStructuredBuffer<float4> MousePosition : register(u0);

Texture2D HeightMap : register(t0);
RWTexture2D<float4> RWHeightMap : register(u1);

RWStructuredBuffer<float4> tempHeightMap : register(u2);

RWTexture2D<float4> SplatMap : register(u3);
Texture2D TempSplatMap : register(t3);

void GetRay(float2 mouse, out float3 dir, out float3 cameraPos)
{
    dir = float3(0, 0, 0);
    cameraPos = float3(0, 0, 0);
    
    float3 mPoint = float3(mouse, 1.0f);
    dir = mul((float3x3) View, mPoint);
    cameraPos = InvView[3].xyz;
}

bool IsIntersectRayToPolygon(float3 orig, float3 dir,
    float3 A, float3 B, float3 C,
    out float2 braycentricUV, out float distance)
{
    float3 P, T, Q;
    float3 E1 = B - A;
    float3 E2 = C - A;

    P = cross(dir, E2);
    float det = 1.0f / dot(E1, P);

    T = orig - A;
    braycentricUV.x = dot(T, P) * det;

    Q = cross(T, E1);
    braycentricUV.y = dot(dir, Q) * det;

    distance = dot(E2, Q) * det;

    return (
      (braycentricUV.x >= 0.0f)
      && (braycentricUV.y >= 0.0f)
      && ((braycentricUV.x + braycentricUV.y) <= 1.0f)
      && (distance >= 0.0f)
    );
}
[numthreads(16, 16, 1)]
void CalcuPickPosition(uint3 DTid : SV_DispatchThreadID)
{

    float2 pos;
    pos.x = MouseScreenPos.x / Projection._11;
    pos.y = MouseScreenPos.y / Projection._22;

    float3 dir, camPos;
    GetRay(pos, dir, camPos);


    uint2 temp;
    HeightMap.GetDimensions(temp.x, temp.y);
    uint2 index = clamp(DTid.xy, uint2(0, 0), temp.xy);

    float3 patch[4];
    float height = HeightMap[index].r * _heightRatio;
    patch[0] = float3(index.x, height, index.y);
    height = HeightMap[uint2(index.x + 1, index.y)].r * _heightRatio;
    patch[1] = float3(index.x + 1, height, index.y);
    height = HeightMap[uint2(index.x, index.y + 1)].r * _heightRatio;
    patch[2] = float3(index.x, height, index.y + 1);
    height = HeightMap[uint2(index.x + 1, index.y + 1)].r * _heightRatio;
    patch[3] = float3(index.x + 1, height, index.y + 1);

    float t;
    float2 uv;
    if (IsIntersectRayToPolygon(camPos, dir, patch[0], patch[2], patch[1], uv, t))
    {
        float3 location = patch[0] + uv.x * (patch[2] - patch[0]) + uv.y * (patch[1] - patch[0]);
        float len = length(camPos - location);

        //if (MousePosition[0].w > len)
            MousePosition[0] = float4(location, len);

    }
    if (IsIntersectRayToPolygon(camPos, dir, patch[3], patch[1], patch[2], uv, t))
    {
        float3 location = patch[3] + uv.x * (patch[1] - patch[3]) + uv.y * (patch[2] - patch[3]);
        float len = length(camPos - location);

        //if (MousePosition[0].w > len)
            MousePosition[0] = float4(location, len);

    }


    //float u, v, t;
    //if (IntersectTri(camPos, dir, patch[0], patch[2], patch[1], u, v, t))
    //{
    //    float3 location = patch[0] + u * (patch[2] - patch[0]) + v * (patch[1] - patch[0]);
    //    float len = abs(distance(location, camPos));
    //    //float temp = abs(distance(MousePosition[0], camPos));

    //    MousePosition[tempindex++] = float4(location, len);

    //    //if (len <= temp)
    //    //    MousePosition[0] = location;
    //}
    //if (IntersectTri(camPos, dir, patch[3], patch[1], patch[2], u, v, t))
    //{
    //    float3 location = patch[3] + u * (patch[1] - patch[3]) + v * (patch[2] - patch[3]);
    //    float len = abs(distance(location, camPos));
    //    //float temp = abs(distance(MousePosition[0], camPos));

    //    MousePosition[tempindex++] = float4(location, len);

    //    //if (len <= temp)
    //    //    MousePosition[0] = location;
    //}

}
[numthreads(16, 16, 1)]
void CopyHeightMap(uint3 dtid : SV_DispatchThreadID)
{
    float3 position = PickPos;

    uint2 temp;
    HeightMap.GetDimensions(temp.x, temp.y);

    uint2 index = clamp(dtid.xy, uint2(0, 0), temp.xy);

    //정사각형
    if (BrushStyle == 0)
    {
        if (abs(index.x - position.x) <= BrushSize &&
            abs(index.y - position.z) <= BrushSize)
        {
            float color = HeightMap[index].r + HeightAmount;
            tempHeightMap[index.x + index.y * temp.x] = float4(color, color, color, 1);
        }
    }
    if (BrushStyle == 1)
    {
        float dx = index.x - position.x;
        float dy = index.y - position.z;

        float distance = sqrt(dx * dx + dy * dy);

        if (distance <= BrushSize)
        {
            float color = HeightMap[index].r + HeightAmount;
            tempHeightMap[index.x + index.y * temp.x] = float4(color, color, color, 1);

        }
    }
}


[numthreads(16, 16, 1)]
void HeightEdit(uint3 dtid : SV_DispatchThreadID)
{
    float3 position = PickPos;

    uint2 temp;
    RWHeightMap.GetDimensions(temp.x, temp.y);

    uint2 index = clamp(dtid.xy, uint2(0, 0), temp.xy);
 
    //정사각형
    if (BrushStyle == 0)
    {
        if (abs(index.x - position.x) <= BrushSize &&
             abs(index.y - position.z) <= BrushSize)
        {
            float color = tempHeightMap[index.y * temp.x + index.x].r;

            if (color <= 0.001f)
                return;

            RWHeightMap[index.xy] = float4(color, color, color, 1);

        }
    }
    if (BrushStyle == 1)
    {
        float dx = index.x - position.x;
        float dy = index.y - position.z;

        float distance = sqrt(dx * dx + dy * dy);

        if (distance <= BrushSize)
        {
            float color = tempHeightMap[index.y * temp.x + index.x].r;

            if (color <= 0.001f)
                return;

            RWHeightMap[index.xy] = float4(color, color, color, 1);

        }
    }
}


[numthreads(16, 16, 1)]
void CopyALLHeightMap(uint3 dtid : SV_DispatchThreadID)
{
    uint2 temp;
    HeightMap.GetDimensions(temp.x, temp.y);

    uint2 index = clamp(dtid.xy, uint2(0, 0), temp.xy);

    float4 color = HeightMap[index];
    tempHeightMap[index.x + index.y * temp.x] = color;
}

[numthreads(16, 16, 1)]
void Smooth(uint3 dtid : SV_DispatchThreadID)
{
    float3 position = PickPos;

    uint2 temp;
    RWHeightMap.GetDimensions(temp.x, temp.y);

    uint2 index = clamp(dtid.xy, uint2(0, 0), temp.xy);
 
    //복사된 버퍼를 참조해서 rw버퍼에 입력을한다
    //rw버퍼로만 수행할 경우 메모리를 서로 참조하게되서 이상한 결과가 나올것이다

    //정사각형
    if (BrushStyle == 0)
    {
        if (abs(index.x - position.x) <= BrushSize &&
             abs(index.y - position.z) <= BrushSize)
        {
            int num = 0;
            float avg = 0.f;

            //for문이 정상적으로 작동을 안함.. - for Statement 문제인가
            int i = index.x;
            int j = index.y;
            avg += tempHeightMap[(i - 1) + (j - 1) * temp.x].r;
            avg += tempHeightMap[(i) + (j - 1) * temp.x].r;
            avg += tempHeightMap[(i + 1) + (j - 1) * temp.x].r;
            avg += tempHeightMap[(i - 1) + (j) * temp.x].r;
            avg += tempHeightMap[(i) + (j) * temp.x].r;
            avg += tempHeightMap[(i + 1) + (j) * temp.x].r;
            avg += tempHeightMap[(i - 1) + (j + 1) * temp.x].r;
            avg += tempHeightMap[(i) + (j + 1) * temp.x].r;
            avg += tempHeightMap[(i + 1) + (j + 1) * temp.x].r;

            float color = avg / 9;
            RWHeightMap[index] = float4(color, color, color, 1);

        }
    }
}

cbuffer SplatBuffer : register(b2)
{
    int SplatNum;
    float SplatAmount;
    int2 SplatBuffer_Padding;
}

[numthreads(16, 16, 1)]
void Splat(uint3 dtid : SV_DispatchThreadID)
{
    float3 position = PickPos;

    uint2 temp;
    RWHeightMap.GetDimensions(temp.x, temp.y);

    uint2 index = clamp(dtid.xy, uint2(0, 0), temp.xy);

    float spec[4] = { 0,0,0,0 };
    spec[SplatNum] = 1.0f;
    float4 add = float4(spec[0], spec[1], spec[2], spec[3]);
    add *= SplatAmount;

    //정사각형
    if (BrushStyle == 0)
    {
        if (abs(index.x - position.x) <= BrushSize &&
            abs(index.y - position.z) <= BrushSize)
        {

            float4 color = TempSplatMap[index.xy];
            color += add;
            color = clamp(color, float4(0, 0, 0, 0), float4(1, 1, 1, 1));
            SplatMap[index.xy] = color;
        }

    }
    if (BrushStyle == 1)
    {
        float dx = index.x - position.x;
        float dy = index.y - position.z;

        float distance = sqrt(dx * dx + dy * dy);

        if (distance <= BrushSize)
        {
            float4 color = TempSplatMap[index.xy];
            color += add;
            color = clamp(color, float4(0, 0, 0, 0), float4(1, 1, 1, 1));
            SplatMap[index.xy] = color;
        }
    }

}