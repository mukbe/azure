#include "stdafx.h"
#include "QuadTreeSystem.h"

#include "QuadTreeNode.h"
QuadTreeSystem::QuadTreeSystem()
{
	this->rootNode = new QuadTreeNode(3, D3DXVECTOR3(0.f, 0.f, 0.f), D3DXVECTOR3(256.f, 0.f, 256.f));
}


QuadTreeSystem::~QuadTreeSystem()
{
	SafeDelete(rootNode);
}

void QuadTreeSystem::Update()
{

}

void QuadTreeSystem::Render()
{
	QuadTreeNode::_renderingNodeCount = 0;
	rootNode->Render();
}

void QuadTreeSystem::UIRender()
{
	ImGui::Begin("QuadTreeSystem");
	{
		ImGui::Text("DrawNode : %d", reinterpret_cast<int*>(QuadTreeNode::_renderingNodeCount));
	}
	ImGui::End();
}
