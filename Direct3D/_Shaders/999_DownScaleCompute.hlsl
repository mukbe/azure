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
    uint Domain;                //축소된 이미지의 총 픽셀
    uint GroupSize;             //첫번째 디스패치에서 파견된 그룹의 수
    float Adaptation;           //적응 계수(노출효과 계수)
    float fBloomThreshold;      //블룸 임계값 백분율 
    float2 padding;
}

groupshared float SharedPositions[1024]; //중간 결과를 저장할 그룹 공유 메모리

static const float4 Lum_Factor = float4(0.299, 0.587, 0.114, 0); //휘도 연산 값(luminationFactor)


/*************************************************************
DownScale First Pass
라이틴 연산이 끝난 텍스처를 다운 샘플링한다. 
다운 샘플링이 끝나면 다운샘플링 된 픽셀의 평균 휘도값도 같이 구해준다.
**************************************************************/

[numthreads(1024, 1, 1)]
void CSMain0(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID,
    uint3 dispatchThreadId : SV_DispatchThreadID)
{
    //인덱스 구하기
    uint2 curPixel = uint2(dispatchThreadId.x % width, dispatchThreadId.x / width);
   
    float avgLum = 0.0f;
    //Y인덱스 값이 텍스처 사이즈를 벗어나지 않는다면
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
         //평균
        downScaledColor /= 16.0f;
        //1/4 해상도 이미지 저장
        DownScaleTexture[curPixel.xy] = downScaledColor;
        //1/4 이미지의 휘도값 계산
        avgLum = dot(downScaledColor, Lum_Factor);
    }
   
    //평균 휘도 값을 구하기 위해 휘도값을 저장 
    SharedPositions[groupThreadId.x] = avgLum;
    //다음 단계전 동기화
    GroupMemoryBarrierWithGroupSync();

    //1024해상도에서 256해상도로 
    if (groupThreadId.x % 4 == 0)
    {
        //이 단계의 휘도 합계 계산
        float stepAvgLum = avgLum;
        stepAvgLum += dispatchThreadId.x + 1 < Domain ? SharedPositions[groupThreadId.x + 1] : avgLum;
        stepAvgLum += dispatchThreadId.x + 2 < Domain ? SharedPositions[groupThreadId.x + 2] : avgLum;
        stepAvgLum += dispatchThreadId.x + 3 < Domain ? SharedPositions[groupThreadId.x + 3] : avgLum;
        
        //결과 값 저장
        avgLum = stepAvgLum;
        SharedPositions[groupThreadId.x] = stepAvgLum;
    }
    //동기화
    GroupMemoryBarrierWithGroupSync();

    //256해상도에서 64에서
    if (groupThreadId.x % 16 == 0)
    {
        //이 단계의 휘도 합계 계산
        float stepAvgLum = avgLum;
        stepAvgLum += dispatchThreadId.x + 4 < Domain ? SharedPositions[groupThreadId.x + 4] : avgLum;
        stepAvgLum += dispatchThreadId.x + 8 < Domain ? SharedPositions[groupThreadId.x + 8] : avgLum;
        stepAvgLum += dispatchThreadId.x + 12 < Domain ? SharedPositions[groupThreadId.x + 12] : avgLum;

		// Store the results
        avgLum = stepAvgLum;
        SharedPositions[groupThreadId.x] = stepAvgLum;
    }
    //동기화
    GroupMemoryBarrierWithGroupSync();

    //64 - > 16
    if (groupThreadId.x % 64 == 0)
    {
        //휘도 합계 계산
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
        //다음 단계에서 사용할 1D UAV에 최종 값을 씁니다.
        AverageLum1D[groupId.x] = fFinalAvgLum;
    }
}

/*************************************************************
DownScale Second Pass - 1D 평균 값을 단일 값으로 변환
휘도값만을 다운샘플링 하면서 평균 휘도값을 구한다.
**************************************************************/

StructuredBuffer<float> AverageValues1D : register(t1);
StructuredBuffer<float> PrevAvgLum : register(t2);


#define MAX_GROUPS 64
groupshared float SharedAvgFinal[MAX_GROUPS];

[numthreads(MAX_GROUPS, 1, 1)]
void CSMain1(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID,
    uint3 dispatchThreadId : SV_DispatchThreadID)
{
	//공유 메모리를 1D 값으로 채웁니다.
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

