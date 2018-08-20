#pragma once

class SceneNode
{
public:
	SceneNode() {}
	~SceneNode() {}

	virtual void Init() = 0;
	virtual void Release() = 0;
	virtual void PreUpdate() = 0;
	virtual void Update() = 0;
	virtual void PostUpdate() = 0;
private:

};

