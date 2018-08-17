#pragma once
#include "stdafx.h"

struct FbxBlendWeight
{
	D3DXVECTOR4 Indices = D3DXVECTOR4(0, 0, 0, 0);
	D3DXVECTOR4 Weights = D3DXVECTOR4(0, 0, 0, 0);

	void Set(int index, int boneIndex, float weight)
	{
		float i = (float)boneIndex;
		float w = weight;

		switch (index)
		{
		case 0: Indices.x = i; Weights.x = w; break;
		case 1: Indices.y = i; Weights.y = w; break;
		case 2: Indices.z = i; Weights.z = w; break;
		case 3: Indices.w = i; Weights.w = w; break;
		}
	}
};

struct FbxBoneWeights
{
private:
	typedef pair<int, float> Pair;
	vector<Pair> BoneWeights;

public:
	void AddBoneWeight(int boneIndex, float boneWeight)
	{
		if (boneWeight <= 0.0f) return;

		bool bAdd = false;
		vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			if (boneWeight > it->second)
			{
				BoneWeights.insert(it, Pair(boneIndex, boneWeight));
				bAdd = true;

				break;
			}

			it++;
		}//for(it)

		if (bAdd == false)
			BoneWeights.push_back(Pair(boneIndex, boneWeight));
	}

	void AddBoneWeights(FbxBoneWeights& boneWeights)
	{
		vector<Pair>& temp = boneWeights.BoneWeights;

		for (size_t i = 0; i < temp.size(); i++)
			AddBoneWeight(temp[i].first, temp[i].second);
	}

	void GetBlendWeights(FbxBlendWeight& blendWeights)
	{
		int i = 0;
		vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			blendWeights.Set(i, it->first, it->second);
			i++; it++;

			if (i >= 4)
				return;
		}
	}

	void Normalize()
	{
		float totalWeight = 0.0f;

		int i = 0;
		vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			if (i < 4)
			{
				totalWeight += it->second;
				++i; ++it;
			}
			else
				it = BoneWeights.erase(it);
		}//while(it)


		float scale = 1.0f / totalWeight;

		it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			it->second *= scale;
			it++;
		}//while(it)
	}
};