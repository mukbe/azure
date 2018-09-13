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
	void DebugUpdate(D3DXMATRIX matLocal,D3DXMATRIX matFinal);
	virtual void Render(D3DXCOLOR color = D3DXCOLOR(1.f,0.f,0.f,1.f),bool zBufferOff = false)override;

};

