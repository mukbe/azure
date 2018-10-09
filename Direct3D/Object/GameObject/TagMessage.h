#pragma once
#include <string>

struct TagMessage
{
	string name;
	float delayTime;
	void* data;

	TagMessage():name(""),delayTime(0.0f),data(nullptr){}
	TagMessage(string name, float delayTime = 0.0f, void* data = nullptr)
		:name(name), delayTime(delayTime),data(data) {}
};


