#pragma once
class QuadTreeSystem
{
private:
	class QuadTreeNode* rootNode;
public:
	QuadTreeSystem();
	~QuadTreeSystem();

	void Update();
	void Render();
	void UIRender();
};

