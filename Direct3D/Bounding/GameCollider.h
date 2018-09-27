#pragma once
#include "Interfaces/ICloneable.h"
class BoundingBox;
class GameObject;

class GameCollider : public ICloneable
{
public:
	enum ColliderType {RigidBody = 0,Attack,HeatBox,Static,UnKnown};
protected:
	Synthesize(string,name,Name)
	Synthesize(BoundingBox*, boundingBox,Bounding)
	Synthesize(D3DXMATRIX, finalMatrix,FinalMatrix)
	Synthesize(GameObject*, parentObject,ParentObject)
	Synthesize(ColliderType,type,Type)
public:
	GameCollider(class GameObject* parentObject,class BoundingBox* boundingBox);
	virtual ~GameCollider();

	virtual void Clone(void** clone);

	virtual void Update();
	virtual void Render(D3DXCOLOR color = D3DXCOLOR(1.f,0.f,0.f,1.f),bool bZbufferOff = false);

	bool IsIntersect(GameCollider* collider);
	void GetCorners(vector<D3DXVECTOR3>& output);
	void GetWorldCenterRadius(D3DXVECTOR3* pOutCenter, float* pOutRadius);

	static string GetTypeName(int index);
	static void SaveAnimCollider(class BinaryWriter* w,class AnimationCollider* collider);
	static void LoadAnimCollider(class BinaryReader* r,class AnimationCollider* cillider);
	static void SaveCollider(class BinaryWriter* w, class GameCollider* collider);
	static void LoadCollider(class BinaryReader* r, class GameCollider* collider);
};

