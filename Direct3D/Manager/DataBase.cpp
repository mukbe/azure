#include "stdafx.h"
#include "DataBase.h"

SingletonCpp(DataBase)

DataBase::DataBase()
{
	//jsonRoot = new Json::Value();
	//JsonHelper::ReadData(L"LevelEditor.json", jsonRoot);
}


DataBase::~DataBase()
{
	//JsonHelper::WriteData(L"LevelEditor.json", jsonRoot);
	//SafeDelete(jsonRoot);
}
