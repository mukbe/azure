#include "stdafx.h"
#include "ObjectManager.h"
#include "./Object/GameObject/GameObject.h"

SingletonCpp(ObjectManager)

ObjectManager::ObjectManager()
{
	for (int i = 0; i < ObjectType::None; ++i)
	{
		this->objectList.insert(make_pair((ObjectType::Tag)i, make_pair(vector<GameObject*>(),vector<GameObject*>())));
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
		ArrObject staticList = iter->second.first;
		for (UINT i = 0; i < staticList.size(); ++i)
		{
			staticList[i]->Release();
			SafeDelete(staticList[i]);
		}
		ArrObject dynamicList = iter->second.second;
		for (UINT i = 0; i < dynamicList.size(); ++i)
		{
			dynamicList[i]->Release();
			SafeDelete(dynamicList[i]);
		}
	}

	objectList.clear();
}

void ObjectManager::PreUpdate()
{
	ObjectContainerIter iter = this->objectList.begin();
	for (; iter != objectList.end(); ++iter)
	{
		ArrObject dynamicList = iter->second.second;
		for (UINT i = 0; i < dynamicList.size(); ++i)
		{
			dynamicList[i]->PreUpdate();
		}
	}
}

void ObjectManager::Update()
{
	ObjectContainerIter iter = this->objectList.begin();
	for (; iter != objectList.end(); ++iter)
	{
		ArrObject dynamicList = iter->second.second;
		for (UINT i = 0; i < dynamicList.size(); ++i)
		{
			dynamicList[i]->Update();
		}
	}
}

void ObjectManager::PostUpdate()
{
	ObjectContainerIter iter = this->objectList.begin();
	for (; iter != objectList.end(); ++iter)
	{
		ArrObject dynamicList = iter->second.second;
		for (UINT i = 0; i < dynamicList.size(); ++i)
		{
			dynamicList[i]->PostUpdate();
		}
	}
}

void ObjectManager::AddObject(ObjectType::Tag tag, class GameObject* object,bool isStatic)
{
	if (isStatic == true)
		this->objectList[tag].first.push_back(object);
	else 
		this->objectList[tag].second.push_back(object);
}

void ObjectManager::DeleteObject(ObjectType::Tag tag, string name, bool isStatic)
{
	ArrObject* pList;
	if (isStatic)
		pList = &objectList[tag].first;
	else 
		pList = &objectList[tag].second;
	
	for (UINT i = 0; i < pList->size(); ++i)
	{
		if ((*pList)[i]->GetName() == name)
		{
			(*pList)[i]->Release();
			SafeDelete((*pList)[i]);
			pList->erase(pList->begin() + i);
			break;
		}
	}
}

GameObject*  ObjectManager::FindObject(ObjectType::Tag tag, string name, bool isStatic)
{
	ArrObject list;
	if (isStatic)
		list = objectList[tag].first;
	else
		list = objectList[tag].second;

	for (UINT i = 0; i < list.size(); ++i)
	{
		if (list[i]->GetName() == name)
		{
			return list[i];
		}
	}

	return nullptr;
}

vector<GameObject*>  ObjectManager::FindObjects( ObjectType::Tag tag, string name, bool isStatic)
{
	ArrObject list;
	if (isStatic)
		list = objectList[tag].first;
	else
		list = objectList[tag].second;

	 vector<GameObject*> findList;
	 for (UINT i = 0; i <list.size(); ++i)
	 {
		 if (list[i]->GetName() == name)
		 {
			 findList.push_back(list[i]);
		 }
	 }

	 return findList;
}

vector< GameObject* > ObjectManager::GetObjectList( ObjectType::Tag tag, bool isStatic)
{
	if (isStatic)
		return objectList[tag].first;
	else
		return objectList[tag].second;
}
