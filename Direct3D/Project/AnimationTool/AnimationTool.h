#pragma once
class AnimationTool
{
private:
	class ModelAnimPlayer* animation;
public:
	AnimationTool();
	~AnimationTool();
	
	void Init();
	void Release();
	void Update();
	void Render();
	void UIRender();

	void AttachAnimation(class Model* model);
};

