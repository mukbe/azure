/*********************************************************
Compute Animation Shader
**********************************************************/

struct KeyFrameData
{
    matrix transform;
    float4 rotation;
    float3 translation;
    float3 scale;
};


struct BoneData
{
    matrix invTransform;
    int parentBoneIndex;
};

RWStructuredBuffer<KeyFrameData> rwKeyFrameBuffer : register(u0);

RWStructuredBuffer<BoneData> rwBoneDataBuffer : register(u1);

RWStructuredBuffer<matrix> rwSkinAnimationBuffer : register(u2);

RWStructuredBuffer<matrix> rwBoneAnimationBuffer : register(u3);

RWStructuredBuffer<matrix> rwKeyFrameAnimationBuffer : register(u4);


cbuffer WorldBuffer : register(b1)
{
    matrix world;
}

cbuffer AnimationBuffer : register(b10)
{
    float frameFactor;
    int currentKeyFrame;
    int nextKeyFrame;
    int currentAnimationOffset;
    int currentClipTotalFrame;
    int boneCount;
    float2 animationPadding;
}


//현재 본의 키프레임 인덱스 = clipOffset + (본인덱스 * 현재클립의 totalFrame) + currentKeyFrame;

[numthreads(228, 1, 1)]
void ComputeKeyFrame(uint3 DTid : SV_DispatchThreadID)
{
    int idIndex = DTid.x;

    if(idIndex >= boneCount)
        return;

    matrix computeMatrix;
    uint dataIndex = currentAnimationOffset + (idIndex * currentClipTotalFrame) + currentKeyFrame;
    uint nextIndex = currentAnimationOffset + (idIndex * currentClipTotalFrame) + nextKeyFrame;
    matrix currentMatrix = rwKeyFrameBuffer[dataIndex].transform;
    matrix nextMatrix = rwKeyFrameBuffer[nextIndex].transform;
    computeMatrix = lerp(currentMatrix, nextMatrix, frameFactor);
    rwKeyFrameAnimationBuffer[idIndex] = computeMatrix;
}


[numthreads(1, 1, 1)]
void ComputeAnimation(uint3 DTid : SV_DispatchThreadID)
{
    float4x4 mat;
    mat[0] = float4(1, 0, 0, 0);
    mat[1]= float4(0, 1, 0, 0);
    mat[2]= float4(0, 0, 1, 0);
    mat[3]= float4(0, 0, 0, 1);

    [loop]
    for (int i = 0; i < boneCount; i++)
    {
        int parentIndex = rwBoneDataBuffer[i].parentBoneIndex;
        matrix matParentAnimation;
        matrix matAnimation = rwKeyFrameAnimationBuffer[i];

        if (parentIndex < 0.9f)
        {
            matParentAnimation = mat;
        }
        else
        {
            matParentAnimation = rwBoneAnimationBuffer[parentIndex];
        }

        rwBoneAnimationBuffer[i] = mul(matAnimation, matParentAnimation);
        rwSkinAnimationBuffer[i] = mul(rwBoneDataBuffer[i].invTransform, rwBoneAnimationBuffer[parentIndex]);
    }
}