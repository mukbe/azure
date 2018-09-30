#pragma once
#include "DefineHeader.h"
#include <unordered_map>

namespace ObjectType
{
	enum class Tag
	{
		System = 0,Enviroment = 1 ,Unit = 2,Object = 3,Instancing = 4, Ui = 5 , None
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
	class CameraBase* mainCamera;
	Synthesize(bool ,isDebug,IsDebug)
public:
	void Init();
	void Release();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void PreRender();
	void Render();
	void PostRender();
	void DebugRender();

	void AddObject(ObjectType::Type type , ObjectType::Tag tag, class GameObject* object);
	void DeleteObject(ObjectType::Type type, ObjectType::Tag tag, string name);

	class GameObject*  FindObject(ObjectType::Type type, ObjectType::Tag tag, string name);
	vector<class GameObject*>  FindObjects(ObjectType::Type type, ObjectType::Tag tag, string name);
	vector<class GameObject*>* GetObjectList (ObjectType::Type type, ObjectType::Tag tag);

	void ObjectSortingFront(ObjectType::Type type, ObjectType::Tag tag, string name);

	string GetTagName(ObjectType::Tag tag);

	void LoadData(Json::Value* parent);
	void SaveData(Json::Value* parent);

	class CameraBase* GetMainCamera();
	void SetMainCamera(class CameraBase* camera) { this->mainCamera = camera; }
};

#define Objects ObjectManager::Get()
#define MainCamera Objects->GetMainCamera()
