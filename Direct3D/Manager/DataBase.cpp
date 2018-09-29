#include "stdafx.h"
#include "DataBase.h"

SingletonCpp(DataBase)

DataBase::DataBase()
{
	jsonRoot = new Json::Value();
	JsonHelper::ReadData(L"../_Scenes/Scene01/Scene01.json", jsonRoot);
}


DataBase::~DataBase()
{
	SafeDelete(jsonRoot);
}

void DataBase::Save()
{
	Objects->SaveData(jsonRoot);

	JsonHelper::WriteData(L"../_Scenes/Scene01/Scene01.json", jsonRoot);
}

void DataBase::Load()
{
	jsonRoot = new Json::Value();
	JsonHelper::ReadData(L"../_Scenes/Scene01/Scene01.json", jsonRoot);
}


void DataBase::SaveData(wstring file)
{
}

void DataBase::LoadData(wstring file)
{
}
