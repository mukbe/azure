#pragma once
#include "DefineHeader.h"
#include <unordered_map>

namespace ObjectType
{
	enum Enum
	{
		Object, Enemy, Ui, END
	};
}

class ObjectManager
{
	SingletonHeader(ObjectManager)
private:
	typedef vector<class GameObject*>								ArrObject;
	typedef unordered_map<ObjectType::Enum, ArrObject>				ObjectContainer;
	typedef unordered_map<ObjectType::Enum, ArrObject>::iterator	ObjectContainerIter;

	ObjectContainer objectList;
public:
	void Release();
	void Update();

	void AddObject(ObjectType::Enum type, class GameObject* object);
	void DeleteObject(ObjectType::Enum type, string name);

	class GameObject*  FindObject(ObjectType::Enum type, string name);
	vector<class GameObject*>  FindObjects(ObjectType::Enum type, string name);
	vector<class GameObject*> GetObjectList(ObjectType::Enum type);
};

#define Objects ObjectManager::Get()

