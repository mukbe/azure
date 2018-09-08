#include "stdafx.h"
#include "QuadTreeNode.h"

#include "./Bounding/BoundingBox.h"
#include "./Bounding/BoundingFrustum.h"
#include "./View/CameraBase.h"

UINT QuadTreeNode::_renderingNodeCount = 0;

QuadTreeNode::QuadTreeNode(int level , D3DXVECTOR3 minPos, D3DXVECTOR3 maxPos)
	:level(level)
{
	this->boundingBox = new BoundingBox(minPos, maxPos);
	this->SubDevide();
}

QuadTreeNode::QuadTreeNode(QuadTreeNode * parent, CornerType cornerType)
{
	D3DXVECTOR3 parentMin, parentMax,newMinPos,newMaxPos;
	float width, height;

	parentMin = parent->boundingBox->minPos;
	parentMax = parent->boundingBox->maxPos;
	width = parentMax.x - parentMin.x;
	height = parentMax.z - parentMin.z;

	switch (cornerType)
	{
		case QuadTreeNode::CORNER_LT:
		{
			newMinPos = parentMin + D3DXVECTOR3(0.f, 0.f, height * 0.5f);
		}
			break;
		case QuadTreeNode::CORNER_RT:
		{
			newMinPos = parentMin + D3DXVECTOR3(width * 0.5f, 0.f, height * 0.5f);
		}
			break;
		case QuadTreeNode::CORNER_LB:
		{
			newMinPos = parentMin;
		}
			break;
		case QuadTreeNode::CORNER_RB:
		{
			newMinPos = parentMin + D3DXVECTOR3(width * 0.5f, 0.f, 0.f);
		}
			break;
	}

	newMaxPos = newMinPos + D3DXVECTOR3(width * 0.5f, (width + height) * 0.25f, height * 0.5f);

	this->boundingBox = new BoundingBox(newMinPos, newMaxPos);
	this->level = parent->level - 1;
	this->SubDevide();
}

QuadTreeNode::~QuadTreeNode()
{
	SafeDelete(this->boundingBox);
	for (UINT i = 0; i < childs.size(); ++i)
		SafeDelete(childs[i]);
	childs.clear();
	staticObjectList.clear();
}

void QuadTreeNode::Render()
{
	if (CanDeepInTo(level) == true)
	{
		if (this->IsInFrustum(MainCamera->GetFrustum()))
		{
			for (UINT i = 0; i < CornerType::End; ++i)
				childs[i]->Render();
		}
	}
	else
	{
		if (this->IsInFrustum(MainCamera->GetFrustum()))
		{
			QuadTreeNode::_renderingNodeCount++;
			this->boundingBox->RenderAABB();
		}
	}
}

bool QuadTreeNode::SubDevide()
{
	if (CanDeepInTo(level) == false) return false;

	for (UINT i = 0; i < CornerType::End; ++i)
	{
		QuadTreeNode* child = new QuadTreeNode(this, (CornerType)i);
		this->childs.push_back(child);
	}

	return true;
}
//TODO AABB �������� �浹 ���� �� �߰� �۾�
bool QuadTreeNode::IsInFrustum(BoundingFrustum* pFrustum)
{
	return pFrustum->IsSphereInFrustum(boundingBox);
}