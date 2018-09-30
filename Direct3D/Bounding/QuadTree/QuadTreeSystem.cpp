#include "stdafx.h"
#include "QuadTreeSystem.h"

#include "QuadTreeNode.h"
#include "./View/FreeCamera.h"

#include "./Environment/GameMap.h"

QuadTreeSystem::QuadTreeSystem()
	:level(3)
{
	this->name = "QuadTreeSystem";
	GameMap* map = nullptr;
	this->rootNode = new QuadTreeNode(level, map);
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
