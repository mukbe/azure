#define PI 3.14159265359

struct StructuredData2
{
    float2 data;
    uint indexX;
    uint indexY;
};

struct StructuredData4
{
    float4 data;
    uint indexX;
    uint indexY;
};

struct VertexTextureNormal
{
    float3 position;
    float2 uv;
    float3 normal;
};


RWStructuredBuffer<StructuredData2> rwHeightBuffer : register(u0);
StructuredBuffer<StructuredData2> heightBuffer : register(t0);

RWStructuredBuffer<StructuredData4> rwSlopeBuffer : register(u1);
StructuredBuffer<StructuredData4> slopeBuffer : register(t1);

RWStructuredBuffer<StructuredData4> rwDisplacementBuffer : register(u2);
StructuredBuffer<StructuredData4> displacementBuffer : register(t2);

RWStructuredBuffer<float2> rwSpectrum : register(u3);
RWStructuredBuffer<float2> rwSpectrum_conj : register(u4);
RWStructuredBuffer<float> rwButerflyLookUpTabke : register(u5);
RWStructuredBuffer<float> rwDispersionTable : register(u6);

RWStructuredBuffer<VertexTextureNormal> rwVertexBuffer : register(u7);
StructuredBuffer<VertexTextureNormal> vertexBuffer : register(t7);

RWStructuredBuffer<float3> rwOriginVertex : register(u3);

cbuffer OceanComputeBuffer : register(b5)
{
    uint N;
    float length;
    float time;
    int nowPass;
}

float2 FFT(float2 w, float2 input1, float2 input2)
{
    float2 value = input1;
    value.x += w.x * input2.x - w.y * input2.y;
    value.y += w.y * input2.x + w.x * input2.y;

    return value;
}
float4 FFT(float2 w, float4 input1, float4 input2)
{
    float4 value = input1;
    value.x += w.x * input2.x - w.y * input2.y;
    value.y += w.y * input2.x + w.x * input2.y;
    value.z += w.x * input2.z - w.y * input2.w;
    value.w += w.y * input2.z + w.x * input2.w;

    return value;
}

float2 InitSpectrum(float time, uint indexX, uint indexY)
{
    int index = indexY * (N + 1) + indexX;

    float omegat = rwDispersionTable[index] * time;

    float cosValue = cos(omegat);
    float sinValue = sin(omegat);

    float c0a = rwSpectrum[index].x * cosValue - rwSpectrum[index].y * sinValue;
    float c0b = rwSpectrum[index].x * sinValue + rwSpectrum[index].y * cosValue;

    float c1a = rwSpectrum_conj[index].x * cosValue - rwSpectrum_conj[index].y * -sinValue;
    float c1b = rwSpectrum_conj[index].x * -sinValue + rwSpectrum_conj[index].y * cosValue;

    return float2(c0a + c1a, c0b + c1b);
}

[numthreads(1, 1, 1)]
void CSMain0(uint3 DTid : SV_DispatchThreadID)
{
    uint bufferIndex = N * N + DTid.x + DTid.y * N;
    uint bufferIndexX, bufferIndexY;
    bufferIndexY = rwHeightBuffer[bufferIndex].indexY;
    
    if (bufferIndexY == 0)
        return;

    float kz = PI * (2.0f * DTid.y - N) / length;
    float kx = PI * (2.0f * DTid.x - N) / length;
    float len = sqrt(kx * kx + kz * kz);
    float lambda;
    
    float2 c = InitSpectrum(time, DTid.x, DTid.y);

    rwHeightBuffer[bufferIndex].data.x = c.x;
    rwHeightBuffer[bufferIndex].data.y = c.y;

    rwSlopeBuffer[bufferIndex].data.x = -c.y * kx;
    rwSlopeBuffer[bufferIndex].data.y = c.x * kx;

    rwSlopeBuffer[bufferIndex].data.z = -c.y * kz;
    rwSlopeBuffer[bufferIndex].data.w = c.x * kz;

    if (len < 0.000001f)
    {
        rwDisplacementBuffer[bufferIndex].data.x = 0.0f;
        rwDisplacementBuffer[bufferIndex].data.y = 0.0f;
        rwDisplacementBuffer[bufferIndex].data.z = 0.0f;
        rwDisplacementBuffer[bufferIndex].data.w = 0.0f;
    }
    else
    {
        rwDisplacementBuffer[bufferIndex].data.x = -c.y * -(kx / len);
        rwDisplacementBuffer[bufferIndex].data.y = c.x * -(kx / len);
        rwDisplacementBuffer[bufferIndex].data.z = -c.y * -(kz / len);
        rwDisplacementBuffer[bufferIndex].data.w = c.x * -(kz / len);
    }


}


[numthreads(1, 1, 1)]
void CSMain1(uint3 DTid : SV_DispatchThreadID)
{
    int idx = nowPass % 2;
    int idx1 = (nowPass + 1) % 2;

    int bftIdx = 4 * (DTid.y + nowPass * N);

    int X = (int) rwButerflyLookUpTabke[bftIdx + 0];
    int Y = (int) rwButerflyLookUpTabke[bftIdx + 1];
    float2 w;
    w.x = rwButerflyLookUpTabke[bftIdx + 2];
    w.y = rwButerflyLookUpTabke[bftIdx + 3];

    int indexX = DTid.y + DTid.z * N;
    int indexY = idx;
    int index = indexY * N * N + indexX;

    int idxX = N * N * idx1 + (X + DTid.z * N);
    int idxY = N * N * idx1 + (Y + DTid.z * N);

    rwHeightBuffer[index].data = FFT(w, rwHeightBuffer[idxX].data, rwHeightBuffer[idxY].data);
    rwSlopeBuffer[index].data = FFT(w, rwSlopeBuffer[idxX].data, rwSlopeBuffer[idxY].data);
    rwDisplacementBuffer[index].data = FFT(w, rwDisplacementBuffer[idxX].data, rwDisplacementBuffer[idxY].data);
}

[numthreads(1, 1, 1)]
void CSMain2(uint3 DTid : SV_DispatchThreadID)
{
    int idx = nowPass % 2;
    int idx1 = (nowPass + 1) % 2;

    int bftIdx = 4 * (DTid.z + nowPass * N);

    int X = (int) rwButerflyLookUpTabke[bftIdx + 0];
    int Y = (int) rwButerflyLookUpTabke[bftIdx + 1];
    float2 w;
    w.x = rwButerflyLookUpTabke[bftIdx + 2];
    w.y = rwButerflyLookUpTabke[bftIdx + 3];

    int indexX = DTid.y + DTid.z * N;
    int indexY = idx;
    int index = indexY * N * N + indexX;

    int idxX = N * N * idx1 + (DTid.y + X * N);
    int idxY = N * N * idx1 + (DTid.y + Y * N);

    rwHeightBuffer[index].data = FFT(w, rwHeightBuffer[idxX].data, rwHeightBuffer[idxY].data);
    rwSlopeBuffer[index].data = FFT(w, rwSlopeBuffer[idxX].data, rwSlopeBuffer[idxY].data);
    rwDisplacementBuffer[index].data = FFT(w, rwDisplacementBuffer[idxX].data, rwDisplacementBuffer[idxY].data);
}


[numthreads(1, 1, 1)]
void CSMain3(uint3 DTid : SV_DispatchThreadID)
{
    float signs[2] = { 1.0f, -1.0f };
    uint index, index1;
    float lambda = -1.0f;
    float3 normal;

    index = DTid.y * N + DTid.x; // index into buffers
    index1 = DTid.y * (N + 1) + DTid.x; // index into vertices

    int sign = (int) signs[(DTid.x + DTid.y) & 1];

    uint bufferIndex = N * N + index;

    normal = normalize(float3(-rwSlopeBuffer[bufferIndex].data.x * sign, 1.0f, -rwSlopeBuffer[bufferIndex].data.z * sign));

    rwVertexBuffer[index1].position.y = rwHeightBuffer[bufferIndex].data.x * sign;
			
	// displacement
    rwVertexBuffer[index1].position.x = rwOriginVertex[index1].x +
				rwDisplacementBuffer[bufferIndex].data.x * lambda * sign;
    rwVertexBuffer[index1].position.z = rwOriginVertex[index1].z +
				rwDisplacementBuffer[bufferIndex].data.z * lambda * sign;

    rwVertexBuffer[index1].normal = normal;

    uint Nplus1 = N + 1;
    if (DTid.x == 0 && DTid.y == 0)
    {
        uint vertexIndex = index1 + N + Nplus1 * N;
        rwVertexBuffer[index1 + N + (N + 1) * N].position.y = rwHeightBuffer[bufferIndex].data.x * sign;
        rwVertexBuffer[vertexIndex].position.x = rwOriginVertex[vertexIndex].x
					+ rwDisplacementBuffer[bufferIndex].data.x * lambda * sign;
        rwVertexBuffer[vertexIndex].position.z = rwOriginVertex[vertexIndex].z
					+ rwDisplacementBuffer[bufferIndex].data.z * lambda * sign;

        rwVertexBuffer[index1 + N + Nplus1 * N].normal = normal;
    }
    if (DTid.x == 0)
    {
        rwVertexBuffer[index1 + N].position.y = rwHeightBuffer[bufferIndex].data.x * sign;
        rwVertexBuffer[index1 + N].position.x = rwOriginVertex[index1 + N].x + rwDisplacementBuffer[bufferIndex].data.x * lambda * sign;
        rwVertexBuffer[index1 + N].position.z = rwOriginVertex[index1 + N].z + rwDisplacementBuffer[bufferIndex].data.z * lambda * sign;

        rwVertexBuffer[index1 + N].normal = normal;
    }
    if (DTid.y == 0)
    {
        uint vertexIndex = index1 + Nplus1 * N;
        rwVertexBuffer[vertexIndex].position.y = rwHeightBuffer[bufferIndex].data.x * sign;

        rwVertexBuffer[vertexIndex].position.x = rwOriginVertex[vertexIndex].x +
            rwDisplacementBuffer[bufferIndex].data.x * lambda * sign;
        rwVertexBuffer[vertexIndex].position.z = rwOriginVertex[vertexIndex].z +
            rwDisplacementBuffer[bufferIndex].data.z * lambda * sign;

        rwVertexBuffer[vertexIndex].normal = normal;
    }

}