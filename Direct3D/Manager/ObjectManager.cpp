#include "stdafx.h"
#include "ObjectManager.h"
#include "./Object/GameObject/GameObject.h"

SingletonCpp(ObjectManager)

ObjectManager::ObjectManager()
{
	for (UINT i = 0; i < 2; ++i)
	{
		ObjectList list;
		for (UINT j = 0; j < (UINT)ObjectType::Tag::None; ++j)
		{
			ArrObject vList;
			list.insert(make_pair((ObjectType::Tag)j, vList));
		}

		objectContainer.insert(make_pair((ObjectType::Type)i, list));
	}

}


ObjectManager::~ObjectManager()
{
	//this->Release();
}

void ObjectManager::Release()
{
	ObjectContainerIter containerIter = objectContainer.begin();

	for (; containerIter != objectContainer.end(); ++containerIter)
	{
		ObjectList* pList = &containerIter->second;
		ObjectListIter listIter = pList->begin();
		for (; listIter != pList->end(); ++listIter)
		{
			for (UINT i = 0; i < listIter->second.size(); ++i)
			{
				GameObject* object = listIter->second[i];
				object->Release();
				SafeDelete(object);
			}
			listIter->second.clear();
		}
		pList->clear();
	}

	objectContainer.clear();
}

void ObjectManager::PreUpdate()
{
	ObjectList* pList = &objectContainer[ObjectType::Type::Dynamic];
	ObjectListIter listIter = pList->begin();
	for (; listIter != pList->end(); ++listIter)
	{
		for (UINT i = 0; i < listIter->second.size(); ++i)
		{
			if (listIter->second[i]->GetIsLive() == false)
			{
				listIter->second[i]->Release();
				SafeDelete(listIter->second[i]);
				listIter->second.erase(listIter->second.begin() + i);
				--i;
				continue;
			}
			if(listIter->second[i]->GetisActive())
				listIter->second[i]->PreUpdate();
		}
	}
}

void ObjectManager::Update()
{
	ObjectList* pList = &objectContainer[ObjectType::Type::Dynamic];
	ObjectListIter listIter = pList->begin();
	for (; listIter != pList->end(); ++listIter)
	{
		for (UINT i = 0; i < listIter->second.size(); ++i)
		{
			if (listIter->second[i]->GetisActive())
				listIter->second[i]->Update();
		}
	}
	
}

void ObjectManager::PostUpdate()
{
	ObjectList* pList = &objectContainer[ObjectType::Type::Dynamic];
	ObjectListIter listIter = pList->begin();
	for (; listIter != pList->end(); ++listIter)
	{
		for (UINT i = 0; i < listIter->second.size(); ++i)
		{
			if (listIter->second[i]->GetisActive())
				listIter->second[i]->PostUpdate();
		}
	}

}

void ObjectManager::PreRender()
{
}

void ObjectManager::Render()
{
	ObjectList* pList = &objectContainer[ObjectType::Type::Dynamic];
	ObjectListIter listIter = pList->begin();
	for (; listIter != pList->end(); ++listIter)
	{
		for (UINT i = 0; i < listIter->second.size(); ++i)
		{
			if (listIter->second[i]->GetisActive())
				listIter->second[i]->Render();
		}
	}
}

void ObjectManager::PostRender()
{
}

void ObjectManager::AddObject(ObjectType::Type type, ObjectType::Tag tag, class GameObject* object)
{
	objectContainer[type][tag].push_back(object);
}

void ObjectManager::DeleteObject(ObjectType::Type type, ObjectType::Tag tag, string name)
{
	ArrObject* pList = &objectContainer[type][tag];
	for (UINT i = 0; i < pList->size(); ++i)
	{
		GameObject* pObject = (*pList)[i];
		if (name == pObject->GetName())
		{
			pObject->Release();
			SafeDelete(pObject);
			pList->erase(pList->begin() + i);
			return;
		}
	}
	
}

GameObject*  ObjectManager::FindObject(ObjectType::Type type , ObjectType::Tag tag, string name)
{
	ArrObject* pList = &objectContainer[type][tag];
	for (UINT i = 0; i < pList->size(); ++i)
	{
		GameObject* pObject = (*pList)[i];
		if (name == pObject->GetName())
		{
			return pObject;
		}
	}
}

vector<GameObject*>  ObjectManager::FindObjects(ObjectType::Type type, ObjectType::Tag tag, string name)
{
	vector<GameObject*> returnValue;

	ArrObject* pList = &objectContainer[type][tag];
	for (UINT i = 0; i < pList->size(); ++i)
	{
		GameObject* pObject = (*pList)[i];
		if (name == pObject->GetName())
		{
			returnValue.push_back(pObject);
		}
	}

	return returnValue;
}

vector< GameObject* >* ObjectManager::GetObjectList(ObjectType::Type type, ObjectType::Tag tag)
{
	return &objectContainer[type][tag];
}

string ObjectManager::GetTagName(ObjectType::Tag tag)
{
	if (tag == ObjectType::Tag::Enviroment)
		return "Enviroment";
	else if (tag == ObjectType::Tag::View)
		return "View";
	else if (tag == ObjectType::Tag::Object)
		return "Object";
	else if (tag == ObjectType::Tag::Instancing)
		return "Instancing";
	else if (tag == ObjectType::Tag::Ui)
		return "UI";

	return "Unknown";
}
