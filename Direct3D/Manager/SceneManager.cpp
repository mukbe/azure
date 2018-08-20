#include "stdafx.h"
#include "SceneManager.h"


SingletonCpp(SceneManager)


SceneManager::SceneManager()
	:current(nullptr)
{
}


SceneManager::~SceneManager()
{
	Nodes::iterator Iter = scenes.begin();
	for (;Iter != scenes.end(); ++Iter)
	{
		SafeDelete(Iter->second);
		scenes.erase(Iter);
		Iter--;
	}
}

BOOL SceneManager::AddScene(string name, SceneNode * node)
{
	if (scenes.find(name) == scenes.end())
	{//add
		scenes.insert(make_pair(name, node));
		current = node;
	}
	else
	{//fail
		current = FindScene(name);
		return false;
	}

	return true;
}

BOOL SceneManager::ChangeScene(string key)
{
	SceneNode* temp = FindScene(key);
	if (temp != nullptr)
	{
		//�ٲٱ� �� ��� ����
		current->Release();

		current = temp;
		current->Init();
	}
	else
		return false;

	return true;
}

SceneNode * SceneManager::FindScene(string key)
{
	Nodes::iterator Iter = scenes.begin();
	for (;Iter != scenes.end(); ++Iter)
	{
		if (Iter->first == key)
			return Iter->second;
	}
	return nullptr;
}