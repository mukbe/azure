#pragma once

class Program
{
public:
	Program();
	~Program();

	void PreUpdate();
	void Update();
	void PostUpdate();

	void Render();

	void ResizeScreen();
private:
	class Json::Value* jsonRoot;
	class DeferredRenderer* deferred;
	class ShadowRenderer* shadow;
};