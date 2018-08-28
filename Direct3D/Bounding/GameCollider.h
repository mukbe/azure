#pragma once

class BoundingBox;
class GameObject;

class GameCollider
{
public:
	enum ColliderType {RigidBody,Attack,HeatBox,UnKnown};
protected:
	Synthesize(string,name,Name)
	Synthesize(BoundingBox*, boundingBox,Bounding)
	Synthesize(D3DXMATRIX, finalMatrix,FinalMatrix)
	Synthesize(GameObject*, parentObject,ParentObject)
	Synthesize(ColliderType,type,Type)
public:
	GameCollider(class GameObject* parentObject,class BoundingBox* boundingBox);
	virtual ~GameCollider();

	virtual void Update();
	virtual void Render();

	bool IsIntersect(GameCollider* collider);

	static string GetTypeName(int index);
	static void SaveCollider(class BinaryWriter* w,class AnimationCollider* collider);
	static void LoadCollider(class BinaryReader* r, class AnimationCollider* cillider);
};

