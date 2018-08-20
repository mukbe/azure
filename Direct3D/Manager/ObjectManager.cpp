#include "stdafx.h"
#include "ObjectManager.h"
#include "./Object/GameObject/GameObject.h"

SingletonCpp(ObjectManager)

ObjectManager::ObjectManager()
{
	for (int i = 0; i < ObjectType::END; ++i)
	{
		this->objectList.insert(make_pair((ObjectType::Enum)i, vector<GameObject*>()));
	}
}


ObjectManager::~ObjectManager()
{
}

void ObjectManager::Release()
{
	ObjectContainerIter iter = this->objectList.begin();
	for (; iter != objectList.end(); ++iter)
	{
		ArrObject list = iter->second;
		for (UINT i = 0; i < list.size(); ++i)
		{
			list[i]->Release();
			SafeDelete(list[i]);
		}
		iter->second.clear();
	}

	objectList.clear();
}

void ObjectManager::Update()
{
	ObjectContainerIter iter = this->objectList.begin();
	for (; iter != objectList.end(); ++iter)
	{
		ArrObject list = iter->second;
		for (UINT i = 0; i < list.size(); ++i)
		{
			list[i]->Update();
		}
	}
}

void ObjectManager::AddObject(ObjectType::Enum type, GameObject * object)
{
	this->objectList[type].push_back(object);
}

void ObjectManager::DeleteObject(ObjectType::Enum type, string name)
{
	for (UINT i = 0; i < objectList[type].size(); ++i)
	{
		if (objectList[type][i]->GetName() == name)
		{
			objectList[type][i]->Release();
			SafeDelete(objectList[type][i]);
			objectList[type].erase(objectList[type].begin() + i);
			break;
		}
	}
}

GameObject*  ObjectManager::FindObject(ObjectType::Enum type, string name)
{
	for (UINT i = 0; i < objectList[type].size(); ++i)
	{
		if (objectList[type][i]->GetName() == name)
		{
			return objectList[type][i];
		}
	}

	return nullptr;
}

vector<GameObject*>  ObjectManager::FindObjects(ObjectType::Enum type, string name)
{
	 vector<GameObject*> findList;
	 for (UINT i = 0; i < objectList[type].size(); ++i)
	 {
		 if (objectList[type][i]->GetName() == name)
		 {
			 findList.push_back(objectList[type][i]);
		 }
	 }

	 return findList;
}

vector< GameObject* > ObjectManager::GetObjectList(ObjectType::Enum type)
{
	return objectList[type];
}
