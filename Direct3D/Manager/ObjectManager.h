#pragma once
#include "DefineHeader.h"
#include <unordered_map>

namespace ObjectType
{
	enum class Tag
	{
		View = 0,Enviroment ,Object,Instancing, Ui , None
	};

	enum class Type
	{
		Static = 0 ,Dynamic = 1
	};
}


class ObjectManager
{
	friend class Hierarchy;
	SingletonHeader(ObjectManager)
private:
	typedef vector<class GameObject*>								ArrObject;
	typedef unordered_map<ObjectType::Tag, ArrObject>				ObjectList;
	typedef unordered_map<ObjectType::Tag, ArrObject>::iterator		ObjectListIter;
	typedef unordered_map<ObjectType::Type, ObjectList>				ObjectContainer;
	typedef unordered_map<ObjectType::Type, ObjectList>::iterator	ObjectContainerIter;

	ObjectContainer	objectContainer;
private:
	Synthesize(class CameraBase*, mainCamera,MainCamera)
public:
	void Release();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void PreRender();
	void Render();
	void PostRender();

	void AddObject(ObjectType::Type type , ObjectType::Tag tag, class GameObject* object);
	void DeleteObject(ObjectType::Type type, ObjectType::Tag tag, string name);

	class GameObject*  FindObject(ObjectType::Type type, ObjectType::Tag tag, string name);
	vector<class GameObject*>  FindObjects(ObjectType::Type type, ObjectType::Tag tag, string name);
	vector<class GameObject*>* GetObjectList (ObjectType::Type type, ObjectType::Tag tag);

	string GetTagName(ObjectType::Tag tag);
};

#define Objects ObjectManager::Get()
#define MainCamera Objects->GetMainCamera()
