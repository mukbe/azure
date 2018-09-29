#pragma once

class DataBase
{
	SingletonHeader(DataBase)
private:
	Json::Value* jsonRoot;
public:
	Json::Value* GetValue()const { return this->jsonRoot; }

	void Save();
	void Load();

	void SaveData(wstring file);
	void LoadData(wstring file);
};

#define DataBaseManager DataBase::Get()