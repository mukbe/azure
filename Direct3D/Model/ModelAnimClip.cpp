#include "stdafx.h"
#include "ModelAnimClip.h"

#include <unordered_map>

void ModelAnimClip::Clone(void ** clone)
{
	ModelAnimClip* clip = new ModelAnimClip();
	clip->name = name;

	clip->totalFrame = totalFrame;
	clip->frameRate = frameRate;
	clip->defaltFrameRate = defaltFrameRate;

	for (pair<wstring, ModelKeyframe*> key : this->keyframeMap)
	{
		ModelKeyframe* temp = nullptr;
		key.second->Clone((void**)&temp);
		clip->keyframeMap[temp->BoneName] = temp;
	}

	*clone = clip;
}


ModelAnimClip::ModelAnimClip()
	:defaltFrameRate(0.0)
{
}


ModelAnimClip::~ModelAnimClip()
{
	for (pair<wstring, ModelKeyframe*> temp : keyframeMap)
	{
		SafeDelete(temp.second);
	}
	keyframeMap.clear();
}



//void ModelAnimClip::Clone(void ** clone)
//{
//	ModelAnimClip* clip = new ModelAnimClip();
//	clip->name = name;
//
//	clip->totalFrame = totalFrame;
//	clip->frameRate = frameRate;
//	clip->defaltFrameRate = defaltFrameRate;
//
//	for (UINT i = 0; i < this->keyframeMap.size(); ++i)
//	{
//		ModelKeyframe* temp = nullptr;
//		keyframeMap[i]->Clone((void**)&temp);
//		clip->keyframeMap.push_back(temp);
//	}
//
//	*clone = clip;
//}
//
//ModelKeyframe * ModelAnimClip::Keyframe(wstring name)
//{
//	for (UINT i = 0; i < keyframeMap.size(); ++i)
//	{
//		if (keyframeMap[i]->BoneName == name)
//			return keyframeMap[i];
//	}
//
//	return nullptr;
//}
//
//ModelAnimClip::ModelAnimClip()
//	:defaltFrameRate(0.0)
//{
//}
//
//
//ModelAnimClip::~ModelAnimClip()
//{
//	for (UINT i = 0; i < keyframeMap.size(); ++i)
//	{
//		SafeDelete(keyframeMap[i]);
//	}
//	keyframeMap.clear();
//}
