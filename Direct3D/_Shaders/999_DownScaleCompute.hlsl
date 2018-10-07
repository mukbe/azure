/****************************************************
Bloom Effect ComputeShader
*****************************************************/

RWStructuredBuffer<float> AverageLum1D : register(u0);
RWTexture2D<float4> DownScaleTexture : register(u1);
Texture2D DeferredRenderingSRV : register(t0);

cbuffer DownScaleBuffer : register(b1)
{
    uint width;
    uint height;
    uint Domain;                //��ҵ� �̹����� �� �ȼ�
    uint GroupSize;             //ù��° ����ġ���� �İߵ� �׷��� ��
    float Adaptation;           //���� ���(����ȿ�� ���)
    float fBloomThreshold;      //��� �Ӱ谪 ����� 
    float2 padding;
}

groupshared float SharedPositions[1024]; //�߰� ����� ������ �׷� ���� �޸�

static const float4 Lum_Factor = float4(0.299, 0.587, 0.114, 0); //�ֵ� ���� ��(luminationFactor)


/*************************************************************
DownScale First Pass
����ƾ ������ ���� �ؽ�ó�� �ٿ� ���ø��Ѵ�. 
�ٿ� ���ø��� ������ �ٿ���ø� �� �ȼ��� ��� �ֵ����� ���� �����ش�.
**************************************************************/

[numthreads(1024, 1, 1)]
void CSMain0(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID,
    uint3 dispatchThreadId : SV_DispatchThreadID)
{
    //�ε��� ���ϱ�
    uint2 curPixel = uint2(dispatchThreadId.x % width, dispatchThreadId.x / width);
   
    float avgLum = 0.0f;
    //Y�ε��� ���� �ؽ�ó ����� ����� �ʴ´ٸ�
    if (curPixel.y < height)
    {
        int3 fullResPos = int3(curPixel * 4, 0);
        float4 downScaledColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
        //downScaledColor = DeferredRenderingSRV.Load(fullResPos);
        [unroll]
        for (int i = 0; i < 4;i ++)
        {
            [unroll]
            for (int j = 0; j < 4; j++)
            {
                downScaledColor += DeferredRenderingSRV.Load(fullResPos, int2(j, i));
            }
        }
         //���
        downScaledColor /= 16.0f;
        //1/4 �ػ� �̹��� ����
        DownScaleTexture[curPixel.xy] = downScaledColor;
        //1/4 �̹����� �ֵ��� ���
        avgLum = dot(downScaledColor, Lum_Factor);
    }
   
    //��� �ֵ� ���� ���ϱ� ���� �ֵ����� ���� 
    SharedPositions[groupThreadId.x] = avgLum;
    //���� �ܰ��� ����ȭ
    GroupMemoryBarrierWithGroupSync();

    //1024�ػ󵵿��� 256�ػ󵵷� 
    if (groupThreadId.x % 4 == 0)
    {
        //�� �ܰ��� �ֵ� �հ� ���
        float stepAvgLum = avgLum;
        stepAvgLum += dispatchThreadId.x + 1 < Domain ? SharedPositions[groupThreadId.x + 1] : avgLum;
        stepAvgLum += dispatchThreadId.x + 2 < Domain ? SharedPositions[groupThreadId.x + 2] : avgLum;
        stepAvgLum += dispatchThreadId.x + 3 < Domain ? SharedPositions[groupThreadId.x + 3] : avgLum;
        
        //��� �� ����
        avgLum = stepAvgLum;
        SharedPositions[groupThreadId.x] = stepAvgLum;
    }
    //����ȭ
    GroupMemoryBarrierWithGroupSync();

    //256�ػ󵵿��� 64����
    if (groupThreadId.x % 16 == 0)
    {
        //�� �ܰ��� �ֵ� �հ� ���
        float stepAvgLum = avgLum;
        stepAvgLum += dispatchThreadId.x + 4 < Domain ? SharedPositions[groupThreadId.x + 4] : avgLum;
        stepAvgLum += dispatchThreadId.x + 8 < Domain ? SharedPositions[groupThreadId.x + 8] : avgLum;
        stepAvgLum += dispatchThreadId.x + 12 < Domain ? SharedPositions[groupThreadId.x + 12] : avgLum;

		// Store the results
        avgLum = stepAvgLum;
        SharedPositions[groupThreadId.x] = stepAvgLum;
    }
    //����ȭ
    GroupMemoryBarrierWithGroupSync();

    //64 - > 16
    if (groupThreadId.x % 64 == 0)
    {
        //�ֵ� �հ� ���
        float stepAvgLum = avgLum;
        stepAvgLum += dispatchThreadId.x + 16 < Domain ? SharedPositions[groupThreadId.x + 16] : avgLum;
        stepAvgLum += dispatchThreadId.x + 32 < Domain ? SharedPositions[groupThreadId.x + 32] : avgLum;
        stepAvgLum += dispatchThreadId.x + 48 < Domain ? SharedPositions[groupThreadId.x + 48] : avgLum;

        avgLum = stepAvgLum;
        SharedPositions[groupThreadId.x] = stepAvgLum;
    }

    GroupMemoryBarrierWithGroupSync();

    // Downscale from 16 to 4
    if (groupThreadId.x % 256 == 0)
    {
        float stepAvgLum = avgLum;
        stepAvgLum += dispatchThreadId.x + 64 < Domain ? SharedPositions[groupThreadId.x + 64] : avgLum;
        stepAvgLum += dispatchThreadId.x + 128 < Domain ? SharedPositions[groupThreadId.x + 128] : avgLum;
        stepAvgLum += dispatchThreadId.x + 192 < Domain ? SharedPositions[groupThreadId.x + 192] : avgLum;

        avgLum = stepAvgLum;
        SharedPositions[groupThreadId.x] = stepAvgLum;
    }

    GroupMemoryBarrierWithGroupSync(); 

    // Downscale from 4 to 1
    if (groupThreadId.x == 0)
    {
        float fFinalAvgLum = avgLum;
        fFinalAvgLum += dispatchThreadId.x + 256 < Domain ? SharedPositions[groupThreadId.x + 256] : avgLum;
        fFinalAvgLum += dispatchThreadId.x + 512 < Domain ? SharedPositions[groupThreadId.x + 512] : avgLum;
        fFinalAvgLum += dispatchThreadId.x + 768 < Domain ? SharedPositions[groupThreadId.x + 768] : avgLum;
        fFinalAvgLum /= 1024.0;
        //���� �ܰ迡�� ����� 1D UAV�� ���� ���� ���ϴ�.
        AverageLum1D[groupId.x] = fFinalAvgLum;
    }
}

/*************************************************************
DownScale Second Pass - 1D ��� ���� ���� ������ ��ȯ
�ֵ������� �ٿ���ø� �ϸ鼭 ��� �ֵ����� ���Ѵ�.
**************************************************************/

StructuredBuffer<float> AverageValues1D : register(t1);
StructuredBuffer<float> PrevAvgLum : register(t2);


#define MAX_GROUPS 64
groupshared float SharedAvgFinal[MAX_GROUPS];

[numthreads(MAX_GROUPS, 1, 1)]
void CSMain1(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID,
    uint3 dispatchThreadId : SV_DispatchThreadID)
{
	//���� �޸𸮸� 1D ������ ä��ϴ�.
    float avgLum = 0.0f;
    if (dispatchThreadId.x < GroupSize)
    {
        avgLum = AverageValues1D[dispatchThreadId.x];
    }
    SharedAvgFinal[dispatchThreadId.x] = avgLum;

    GroupMemoryBarrierWithGroupSync();

    // Downscale from 64 to 16
    if (dispatchThreadId.x % 4 == 0)
    {
		// Calculate the luminance sum for this step
        float stepAvgLum = avgLum;
        stepAvgLum += dispatchThreadId.x + 1 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 1] : avgLum;
        stepAvgLum += dispatchThreadId.x + 2 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 2] : avgLum;
        stepAvgLum += dispatchThreadId.x + 3 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 3] : avgLum;
		
		// Store the results
        avgLum = stepAvgLum;
        SharedAvgFinal[dispatchThreadId.x] = stepAvgLum;
    }

    GroupMemoryBarrierWithGroupSync(); // Sync before next step

	// Downscale from 16 to 4
    if (dispatchThreadId.x % 16 == 0)
    {
		// Calculate the luminance sum for this step
        float stepAvgLum = avgLum;
        stepAvgLum += dispatchThreadId.x + 4 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 4] : avgLum;
        stepAvgLum += dispatchThreadId.x + 8 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 8] : avgLum;
        stepAvgLum += dispatchThreadId.x + 12 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 12] : avgLum;

		// Store the results
        avgLum = stepAvgLum;
        SharedAvgFinal[dispatchThreadId.x] = stepAvgLum;
    }

    GroupMemoryBarrierWithGroupSync(); // Sync before next step

	// Downscale from 4 to 1
    if (dispatchThreadId.x == 0)
    {
		// Calculate the average luminace
        float fFinalLumValue = avgLum;
        fFinalLumValue += dispatchThreadId.x + 16 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 16] : avgLum;
        fFinalLumValue += dispatchThreadId.x + 32 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 32] : avgLum;
        fFinalLumValue += dispatchThreadId.x + 48 < GroupSize ? SharedAvgFinal[dispatchThreadId.x + 48] : avgLum;
        fFinalLumValue /= 64.0;

		// Calculate the adaptive luminance
        float fAdaptedAverageLum = lerp(PrevAvgLum[0], fFinalLumValue, Adaptation);

		// Store the final value
        AverageLum1D[0] = max(fAdaptedAverageLum, 0.0001);
    }
}

