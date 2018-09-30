#pragma once

class FactoryManager
{
	SingletonHeader(FactoryManager)
public:

	void Create(string name,Json::Value value);
	void CreateInstanceRenderer(string name, Json::Value value);
};

