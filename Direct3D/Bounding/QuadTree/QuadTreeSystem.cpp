#include "stdafx.h"
#include "QuadTreeSystem.h"

#include "QuadTreeNode.h"
#include "./View/FreeCamera.h"
QuadTreeSystem::QuadTreeSystem()
	:level(3)
{
	this->name = "QuadTreeSystem";
	this->rootNode = new QuadTreeNode(level, D3DXVECTOR3(0.f, 0.f, 0.f), D3DXVECTOR3(256.f, 0.f, 256.f));
}


QuadTreeSystem::~QuadTreeSystem()
{
	SafeDelete(rootNode);
}

void QuadTreeSystem::Update()
{
	QuadTreeNode::_renderingNodeCount = 0;
	rootNode->Update(MainCamera->GetFrustum());
}

void QuadTreeSystem::Render()
{

}

void QuadTreeSystem::DebugRender()
{
	rootNode->Render();
}

void QuadTreeSystem::UIRender()
{

	ImGui::Text("QuadTreeNode : %d", reinterpret_cast<int*>(QuadTreeNode::_renderingNodeCount));
	
}

void QuadTreeSystem::AddObject(StaticObject * object)
{
	rootNode->AddObject(object);
}

UINT QuadTreeSystem::GetNodeCount()
{
	return pow(4, level);
}
