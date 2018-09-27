#pragma once

class DataBase
{
	SingletonHeader(DataBase)
private:
	Json::Value* jsonRoot;
public:
	Json::Value* GetValue()const { return this->jsonRoot; }
};

#define DataBaseManager DataBase::Get()