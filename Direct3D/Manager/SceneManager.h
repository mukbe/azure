#pragma once
#include "SceneNode.h"
#include "ModuleHeader.h"
#include <unordered_map>

class SceneManager
{
	SingletonHeader(SceneManager)
private:
	typedef unordered_map<string, class SceneNode*> Nodes;
public:
	SceneManager();
	~SceneManager();

	BOOL AddScene(string name, SceneNode* node);
	BOOL ChangeScene(string key);

private:
	Nodes scenes;
	SceneNode* current;

	SceneNode* FindScene(string key);
};

#define Scenes  SceneManager::Get()

