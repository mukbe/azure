#include "stdafx.h"
#include "SceneManager.h"


SingletonCpp(SceneManager)


SceneManager::SceneManager()
	:current(nullptr),isTool(true)
{
}


SceneManager::~SceneManager()
{
	Nodes::iterator Iter = scenes.begin();
	for (;Iter != scenes.end(); ++Iter)
	{
		SafeDelete(Iter->second);
	}
	scenes.clear();
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

void SceneManager::PreUpdate()
{
	//���� ���Ǹ� ���ؼ� �ƿ�
	//if (Keyboard::Get()->Down(VK_F1))
	//{
	//	Nodes::iterator Iter = scenes.begin();

	//	current = Iter->second;
	//	current->Init();
	//}
	//else if (Keyboard::Get()->Down(VK_F2))
	//{
	//	Nodes::iterator Iter = scenes.begin();
	//	Iter++;

	//	current = Iter->second;
	//	current->Init();
	//}
	//else if (Keyboard::Get()->Down(VK_F3))
	//{
	//	Nodes::iterator Iter = scenes.begin();
	//	Iter++;
	//	Iter++;

	//	current = Iter->second;
	//	current->Init();

	//}

	if (current != nullptr)
		current->PreUpdate();
}

void SceneManager::Update()
{
	if (current != nullptr)
		current->Update();
}

void SceneManager::PostUpdate()
{
	if (current != nullptr)
		current->PostUpdate();
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
