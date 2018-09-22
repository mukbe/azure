#pragma once
#include "SceneNode.h"
#include <unordered_map>

class SceneManager
{
	SingletonHeader(SceneManager)
	Synthesize(bool,isTool,IsTool)
private:
	typedef unordered_map<string, class SceneNode*> Nodes;
public:
	BOOL AddScene(string name, SceneNode* node);
	BOOL ChangeScene(string key);

	void PreUpdate();
	void Update();
	void PostUpdate();

private:
	Nodes scenes;
	SceneNode* current;

	SceneNode* FindScene(string key);
};

#define Scenes  SceneManager::Get()

