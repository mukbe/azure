#pragma once


class ResourceManager
{
	SingletonHeader(ResourceManager)
private:
	typedef unordered_map<string, class Texture*> TextureContainer;
public:

	void UIRender();
private:
	bool isShow;
	TextureContainer textrues;
};


