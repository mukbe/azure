#pragma once
#include "DefineHeader.h"
#include <unordered_map>

namespace ObjectType
{
	enum Enum
	{
		NORMAL, ENEMY, UI, END
	};
}

class ObjectManager
{
	SingletonHeader(ObjectManager)
private:
	typedef vector<class GameObject*>								ArrObject;
	typedef unordered_map<ObjectType::Enum, ArrObject>				ObjectContainer;
	typedef unordered_map<ObjectType::Enum, ArrObject>::iterator	ObjectContainerIter;
public:

	void Release();
	void Update();
};

#define Objects ObjectManager::Get()

