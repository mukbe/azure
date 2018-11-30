#include "stdafx.h"
#include "ObjectManager.h"
#include "./Object/GameObject/GameObject.h"
#include "./View/FreeCamera.h"

SingletonCpp(ObjectManager)

ObjectManager::ObjectManager()
	:isDebug(false),mainCamera(nullptr)
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

	RenderManager::Get()->AddRender("ObjectShadowRender", bind(&ObjectManager::ShadowRender, this),RenderType::Shadow);

}


ObjectManager::~ObjectManager()
{
	//this->Release();
}

void ObjectManager::Init()
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
				listIter->second[i]->Init();
			}
		}
	}
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

void ObjectManager::ShadowRender()
{
	ObjectList* pList = &objectContainer[ObjectType::Type::Dynamic];
	ObjectListIter listIter = pList->begin();
	for (; listIter != pList->end(); ++listIter)
	{
		for (UINT i = 0; i < listIter->second.size(); ++i)
		{
			if (listIter->second[i]->GetisActive())
				listIter->second[i]->ShadowRender();
		}
	}
	
}

void ObjectManager::Render()
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
				if (listIter->second[i]->GetisActive())
					listIter->second[i]->Render();
			}
		}
	}
}

void ObjectManager::PostRender()
{
}

void ObjectManager::DebugRender()
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
				if (listIter->second[i]->GetisActive())
					listIter->second[i]->DebugRender();
			}
		}
	}
}

void ObjectManager::AlphaRender()
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
				if (listIter->second[i]->GetisActive())
					listIter->second[i]->AlphaRender();
			}
		}
	}
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

	return nullptr;
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
	vector<GameObject*>* returnPtr = &objectContainer[type][tag];
	return returnPtr;
}

void ObjectManager::ObjectSortingFront(ObjectType::Type type, ObjectType::Tag tag, string name)
{
	vector<GameObject*>* pList = &objectContainer[type][tag];
	GameObject* temp = nullptr;
	for (UINT i = 0; i < pList->size(); ++i)
	{
		if ((*pList)[i]->GetName() == name)
		{
			temp = (*pList)[i];
			pList->erase(pList->begin() + i);
			break;
		}
	}
	vector<GameObject*> newList;
	newList.push_back(temp);
	for (UINT i = 0; i < pList->size(); ++i)
	{
		if ((*pList)[i]->GetName() == name)
			continue;
		newList.push_back((*pList)[i]);
	}

	objectContainer[type][tag] = newList;
}

string ObjectManager::GetTagName(ObjectType::Tag tag)
{
	if (tag == ObjectType::Tag::Enviroment)
		return "Enviroment";
	else if (tag == ObjectType::Tag::System)
		return "System";
	else if (tag == ObjectType::Tag::Object)
		return "Object";
	else if (tag == ObjectType::Tag::Unit)
		return "Unit";
	else if (tag == ObjectType::Tag::Instancing)
		return "Instancing";
	else if (tag == ObjectType::Tag::Ui)
		return "UI";

	return "Unknown";
}

void ObjectManager::LoadData(Json::Value * parent)
{
	Json::Value value = (*parent)["ObjectManager"];
	Json::ValueIterator typeIter = value.begin();
	for (; typeIter != value.end(); typeIter++)
	{
		Json::Value typeValue = (*typeIter);
		Json::ValueIterator tagIter = typeValue.begin();
		for (; tagIter != typeValue.end(); ++tagIter)
		{
			Json::Value tagValue = (*tagIter);
			Json::ValueIterator listIter = tagValue.begin();
			for (; listIter != tagValue.end(); ++listIter)
			{
				Json::Value objectValue = (*listIter);
				string name;
				JsonHelper::GetValue(objectValue, "Name", name);
				FactoryManager::Get()->Create(name,objectValue);
			}
		}

	}

}

void ObjectManager::SaveData(Json::Value * parent)
{
	Json::Value value;
	{
		ObjectContainerIter containerIter = objectContainer.begin();
		for (; containerIter != objectContainer.end(); ++containerIter)
		{
			Json::Value objectTypeValue;

			ObjectListIter listIter = containerIter->second.begin();
			for (; listIter != containerIter->second.end(); ++listIter)
			{
				Json::Value listValue;
				{
					for (UINT i = 0; i < listIter->second.size(); ++i)
					{
						listIter->second[i]->SaveData(&listValue);

					}
				}
				objectTypeValue[this->GetTagName(listIter->first).c_str()] = listValue;
			}

			if (containerIter->first == ObjectType::Type::Static)
				value["Static"] = objectTypeValue;
			else
				value["Dynamic"] = objectTypeValue;
		}
	}
	(*parent)["ObjectManager"] = value;
}

CameraBase * ObjectManager::GetMainCamera()
{
	//if (mainCamera == nullptr)
	//{
	//	mainCamera = new FreeCamera;
	//	this->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::System, mainCamera);
	//	this->ObjectSortingFront(ObjectType::Type::Dynamic, ObjectType::Tag::System, "Camera");
	//}

	return mainCamera;
}
