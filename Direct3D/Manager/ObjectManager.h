#pragma once
#include "DefineHeader.h"
#include <unordered_map>

namespace ObjectType
{
	enum Tag
	{
		Enviroment = 0,Object, Unit, Ui, None
	};
}

//ObjectPair -- first == StaticObject
//			 -- second == DynamicObject


class ObjectManager
{
	SingletonHeader(ObjectManager)
private:
	typedef vector<class GameObject*>								ArrObject;
	typedef pair<ArrObject, ArrObject>								ObjectPair;
	typedef unordered_map<ObjectType::Tag, ObjectPair>				ObjectContainer;
	typedef unordered_map<ObjectType::Tag, ObjectPair>::iterator	ObjectContainerIter;

	ObjectContainer		objectList;
private:
	Synthesize(class CameraBase*, mainCamera,MainCamera)
public:
	void Release();
	void PreUpdate();
	void Update();
	void PostUpdate();

	void AddObject(ObjectType::Tag tag, class GameObject* object,bool isStatic);
	void DeleteObject(ObjectType::Tag tag, string name, bool isStatic);

	class GameObject*  FindObject(ObjectType::Tag tag, string name, bool isStatic);
	vector<class GameObject*>  FindObjects(ObjectType::Tag tag, string name, bool isStatic);
	vector<class GameObject*> GetObjectList (ObjectType::Tag tag, bool isStatic);
};

#define Objects ObjectManager::Get()
#define MainCamera Objects->GetMainCamera()
