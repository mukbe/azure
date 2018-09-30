#include "stdafx.h"
#include "QuadTreeNode.h"

#include "./Bounding/BoundingBox.h"
#include "./Bounding/BoundingSphere.h"
#include "./Bounding/BoundingFrustum.h"
#include "./Bounding/GameCollider.h"

#include "./View/CameraBase.h"
#include "./Object/StaticObject/StaticObject.h"
#include "./Project/TerrainTool/Terrain.h"
#include "./Environment/GameMap.h"

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

QuadTreeNode::QuadTreeNode(int level, GameMap * pMap)
	:level(level)
{
	D3DXVECTOR3 min, max;
	min.x = min.z = 0.0f;
	max.x = max.z = 256.0f;
	min.y = max.y = 0.0f;
	this->boundingBox = new BoundingBox(min, max);
	this->SubDevide();
}

QuadTreeNode::QuadTreeNode(QuadTreeNode * pParent, CornerType cornerType, GameMap * pMap)
{
	D3DXVECTOR3 parentMin, parentMax, newMinPos, newMaxPos;
	float width, height;

	parentMin = pParent->boundingBox->minPos;
	parentMax = pParent->boundingBox->maxPos;
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

	newMaxPos = newMinPos + D3DXVECTOR3(width * 0.5f, 0.0f, height * 0.5f);

	this->boundingBox = new BoundingBox(newMinPos, newMaxPos);
	this->level = pParent->level - 1;
	this->SubDevide();

	if (this->level == 0)
	{
		D3DXVECTOR2 heightData;
		//heightData = pMap->GetMinMax(newMinPos.x,newMinPos.z);
		this->boundingBox->minPos.y = heightData.x;
		this->boundingBox->maxPos.y = heightData.y;
		D3DXVECTOR3 center = (boundingBox->minPos + boundingBox->maxPos) / 2.0f;
		this->boundingBox->halfSize = center - boundingBox->minPos;
	}
}

QuadTreeNode::~QuadTreeNode()
{
	SafeDelete(this->boundingBox);
	for (UINT i = 0; i < childs.size(); ++i)
		SafeDelete(childs[i]);
	childs.clear();
	objectList.clear();
}

void QuadTreeNode::UpdateHeightData(GameMap * pMap)
{
	if (childs.empty())
		return;
	else
	{
		childs[0]->UpdateHeightData(pMap);
		childs[1]->UpdateHeightData(pMap);
		childs[2]->UpdateHeightData(pMap);
		childs[3]->UpdateHeightData(pMap);
	}

	float minHeight, maxHeight;
	minHeight = childs[0]->boundingBox->minPos.y;
	maxHeight = childs[0]->boundingBox->maxPos.y;
	for (UINT i = 1; i < childs.size(); ++i)
	{
		//if(minHeight <)
	}
}

void QuadTreeNode::Update(class BoundingFrustum* pFrustum)
{
	if (CanDeepInTo(level) == false)
	{
		bool isInFrustum = this->IsInFrustum(pFrustum);
		if (isInFrustum)
		{
			for (UINT i = 0; i < objectList.size(); ++i)
				objectList[i]->SetIsRender(true);

			QuadTreeNode::_renderingNodeCount++;
		}
	}
	else
	{
		bool isInFrustum = this->IsInFrustum(pFrustum);
		if (isInFrustum)
		{
			childs[0]->Update(pFrustum);
			childs[1]->Update(pFrustum);
			childs[2]->Update(pFrustum);
			childs[3]->Update(pFrustum);
		}
	}
}

void QuadTreeNode::Render()
{
	if (CanDeepInTo(level) == false)
	{
		if (this->IsInFrustum(MainCamera->GetFrustum()))
		{
			this->boundingBox->RenderAABB();
		}
	}
	else
	{
		if (this->IsInFrustum(MainCamera->GetFrustum()))
		{
			childs[0]->Render();
			childs[1]->Render();
			childs[2]->Render();
			childs[3]->Render();
		}
	}
}

void QuadTreeNode::AddObject(StaticObject * object)
{
	if (CanDeepInTo(level) == false)
	{
		bool intersects = false;
		vector<GameCollider*> list = object->GetColliderList();
		for (UINT i = 0; i < list.size(); ++i)
		{
			D3DXVECTOR3 worldCenter;
			float radius;
			list[i]->GetWorldCenterRadius(&worldCenter, &radius);
			BoundingSphere sphere(worldCenter, radius);
			intersects = this->boundingBox->Intersects(sphere);
		}

		if (intersects)
		{
			this->objectList.push_back(object);
		}
	}
	else
	{
		bool intersects = false;
		vector<GameCollider*> list = object->GetColliderList();
		for (UINT i = 0; i < list.size(); ++i)
		{
			D3DXVECTOR3 worldCenter;
			float radius;
			list[i]->GetWorldCenterRadius(&worldCenter, &radius);
			BoundingSphere sphere(worldCenter, radius);
			intersects = this->boundingBox->Intersects(sphere);
		}

		if (intersects)
		{
			childs[0]->AddObject(object);
			childs[1]->AddObject(object);
			childs[2]->AddObject(object);
			childs[3]->AddObject(object);
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
//TODO AABB 프러스텀 충돌 구현 후 추가 작업
bool QuadTreeNode::IsInFrustum(BoundingFrustum* pFrustum)
{
	return pFrustum->IsSphereInFrustum(boundingBox);
}

void QuadTreeNode::SetIsInfrustum(bool b)
{
	if (CanDeepInTo(level) == true)
	{
		
	}
	else
	{
		childs[0]->SetIsInfrustum(false);
		childs[1]->SetIsInfrustum(false);
		childs[2]->SetIsInfrustum(false);
		childs[3]->SetIsInfrustum(false);
	}
}
