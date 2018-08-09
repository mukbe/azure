#pragma once

class Program
{
public:
	Program();
	~Program();

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen();
private:
	class FreeCamera* freeCamera;
	class Json::Value* jsonRoot;
};