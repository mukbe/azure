#pragma once
#include "GameCollider.h"
class ModelAnimPlayer;
class AnimationCollider : public GameCollider
{
private:
	Synthesize(int, parentBoneIndex, ParentBoneIndex)
	Synthesize(D3DXMATRIX, localMatrix, localMatrix)
	Synthesize(ModelAnimPlayer*, animation, Animation)
public:
	AnimationCollider(class GameObject* parentObject,class ModelAnimPlayer* animation);
	virtual ~AnimationCollider();

	virtual void Update()override;
	virtual void Render()override;
};

