#pragma once
#include "ToolBase.h"

class GameObject;
class Hierarchy : public ToolBase
{
private:
	class Inspector* inspector;
private:		//View
	class FreeCamera *freeCamera;
private:		//Enviroment
	class Scattering* scattering;
	class Ocean* ocean;
public:
	Hierarchy(class ToolScene* toolScene);
	~Hierarchy();

	virtual void Init();
	virtual void Release();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();
	virtual void Render();
	virtual void PreRender();
	virtual void UIRender();

	virtual void SaveData();
	virtual void LoadData();
private:
	void CreateUnit(string name);
	void CreateEnviroment(string name);
	void CreaetInstanceRenderer(wstring fileName = L"");
	void CreateEffect(string name);
};

