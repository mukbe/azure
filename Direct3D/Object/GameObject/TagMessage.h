#pragma once
#include "stdafx.h"

struct TagMessage
{
	string name;
	float delayTime;

	TagMessage():name(""),delayTime(0.0f){}
	TagMessage(string name, float delayTime = 0.0f):name(name),delayTime(delayTime) {}
};


struct Message_Int : public TagMessage
{
	int intData;
};

struct Message_UINT : public TagMessage
{
	UINT UintData;
};

struct Message_Float : public TagMessage
{
	float floatData;
};

struct Message_String : public TagMessage
{
	string stringData;
};

struct Message_GameObject : public TagMessage
{
	class GameObject* targetData;
};


struct Message_GameObjectList : public TagMessage
{
	vector<class GameObject*> targetListData;
};

struct Message_Vector3 : public TagMessage
{
	D3DXVECTOR3 vector3Data;
};

struct Message_Vector2 : public TagMessage
{
	D3DXVECTOR2 vector2Data;
};

struct Message_Matrix : public TagMessage
{
	D3DXMATRIX matrixData;
};