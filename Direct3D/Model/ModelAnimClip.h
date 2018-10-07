#pragma once
#include "../Interfaces/ICloneable.h"
#include <unordered_map>
struct ModelKeyframeData
{
	D3DXMATRIX Transform;
	D3DXQUATERNION Rotation;
	D3DXVECTOR3 Translation;
	D3DXVECTOR3 Scale;
};

struct ModelKeyframe : public ICloneable
{
	wstring BoneName;
	vector<ModelKeyframeData> Datas;

	void Clone(void** clone)
	{
		ModelKeyframe* frame = new ModelKeyframe();

		frame->BoneName = BoneName;
		for (ModelKeyframeData temp : Datas)
		{
			ModelKeyframeData data;
			data.Rotation = temp.Rotation;
			data.Transform = temp.Transform;
			data.Translation = temp.Translation;
			data.Scale = temp.Scale;

			frame->Datas.push_back(data);
		}

		*clone = frame;
	}
};

class ModelAnimClip : public ICloneable
{
public:
	friend class Model;
	friend class Models;

public:
	void Clone(void** clone);

	wstring Name() { return name; }
	int TotalFrame() { return totalFrame; }
	float FrameRate() { return frameRate; }
	void SetName(wstring name) { this->name = name; }
	class ModelKeyframe* Keyframe(wstring name){ return keyframeMap[name]; }
	unordered_map<wstring, class ModelKeyframe*> KeyFrameMap()const { return this->keyframeMap; }
	//vector<class ModelKeyframe*> KeyFrameMap()const { return this->keyframeMap; }
	//class ModelKeyframe* Keyframe(wstring name);
public:
	ModelAnimClip();
	~ModelAnimClip();
private:
	wstring name;

	int totalFrame;
	float frameRate;
	float defaltFrameRate;

	// Bone Name, ModelKeyframe
	// 키프레임: [ 현재 클립의 각 프레임별 Bone의 Transform 정보 ]
	// keyframeMap[boneName] => ModelKeyframe
	// ModelKeyframe[10] => 현재 클립에서 해당 Bone의 10프레임의 Transform 정보
	//vector<class ModelKeyframe*> keyframeMap;
	unordered_map<wstring,class ModelKeyframe*> keyframeMap;
};
