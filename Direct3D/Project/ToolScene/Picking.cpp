#include "stdafx.h"
#include "Picking.h"

#include "ToolScene.h"
#include "Inspector.h"

#include "./Bounding/GameCollider.h"
#include "./Bounding/Ray.h"
#include "./Bounding/BoundingSphere.h"

#include "./Object/StaticObject/StaticObject.h"

#include "./Utilities/Transform.h"
#include "./Utilities/Math.h"

#include "./View/FreeCamera.h"

Picking::Picking(ToolScene * toolScene)
	:ToolBase(toolScene)
{
}

Picking::~Picking()
{
}

void Picking::Init()
{
	this->inspector = toolScene->GetTool<Inspector>("Inspector");
}

void Picking::Release()
{
}

void Picking::PreUpdate()
{
}

void Picking::Update()
{
	if (ImGui::IsMouseHoveringAnyWindow() == false)
	{
		if (KeyCode->Down(VK_LBUTTON) && !KeyCode->Press(VK_LSHIFT) && !KeyCode->Press(VK_CONTROL))
		{
			//   0-------1
			//  /|      /|
			// 4-------5 |
			// | 3-----|-2
			// |/      |/
			// 7-------6 
			vector<GameObject*>* staticListPtr = Objects->GetObjectList(ObjectType::Type::Dynamic, ObjectType::Tag::Object);
			Ray ray = MainCamera->GetRay();
			vector<GameObject*> pickList;
			for (UINT i = 0; i < staticListPtr->size(); ++i)
			{
				StaticObject* object = (StaticObject*)((*staticListPtr)[i]);
				vector<GameCollider*> colliderList = object->GetColliderList();
				for (UINT j = 0; j < colliderList.size(); ++j)
				{
					D3DXVECTOR3 center;
					float radius;
					colliderList[j]->GetWorldCenterRadius(&center, &radius);
					BoundingSphere sphere(center, radius);
					float result;
					if (sphere.Intersects(ray, &result))
					{
						float padding;
						vector<D3DXVECTOR3> corners;
						colliderList[j]->GetCorners(corners);
						if (D3DXIntersectTri(&corners[4], &corners[5], &corners[7], &ray.origin, &ray.direction, &padding, &padding, &padding)
							|| D3DXIntersectTri(&corners[7], &corners[5], &corners[6], &ray.origin, &ray.direction, &padding, &padding, &padding))
						{
							pickList.push_back((*staticListPtr)[i]);
							break;
						}
						//µÞ¸é
						if (D3DXIntersectTri(&corners[0], &corners[1], &corners[3], &ray.origin, &ray.direction, &padding, &padding, &padding)
							|| D3DXIntersectTri(&corners[3], &corners[1], &corners[2], &ray.origin, &ray.direction, &padding, &padding, &padding))
						{
							pickList.push_back((*staticListPtr)[i]);
							break;
						}
						//¿ÞÂÊ
						if (D3DXIntersectTri(&corners[7], &corners[4], &corners[0], &ray.origin, &ray.direction, &padding, &padding, &padding)
							|| D3DXIntersectTri(&corners[7], &corners[0], &corners[3], &ray.origin, &ray.direction, &padding, &padding, &padding))
						{
							pickList.push_back((*staticListPtr)[i]);
							break;
						}
						//¿ìÃø
						if (D3DXIntersectTri(&corners[6], &corners[5], &corners[1], &ray.origin, &ray.direction, &padding, &padding, &padding)
							|| D3DXIntersectTri(&corners[6], &corners[1], &corners[2], &ray.origin, &ray.direction, &padding, &padding, &padding))
						{
							pickList.push_back((*staticListPtr)[i]);
							break;
						}
						//À­¸é
						if (D3DXIntersectTri(&corners[0], &corners[1], &corners[4], &ray.origin, &ray.direction, &padding, &padding, &padding)
							|| D3DXIntersectTri(&corners[4], &corners[1], &corners[5], &ray.origin, &ray.direction, &padding, &padding, &padding))
						{
							pickList.push_back((*staticListPtr)[i]);
							break;
						}
						//¾Æ·¡¸é
						if (D3DXIntersectTri(&corners[3], &corners[2], &corners[7], &ray.origin, &ray.direction, &padding, &padding, &padding)
							|| D3DXIntersectTri(&corners[7], &corners[2], &corners[6], &ray.origin, &ray.direction, &padding, &padding, &padding))
						{
							pickList.push_back((*staticListPtr)[i]);
							break;
						}
					}
				}
			}
			if (pickList.empty() == false)
			{
				std::sort(pickList.begin(), pickList.end(), [](GameObject* a, GameObject* b)
				{
					D3DXVECTOR3 origin = MainCamera->GetTransform()->GetWorldPosition();
					float disA = D3DXVec3Length(&(origin - a->GetTransform()->GetWorldPosition()));
					float disB = D3DXVec3Length(&(origin - b->GetTransform()->GetWorldPosition()));

					if (disA < disB)return true;
					return false;
				});

				inspector->SetTargetObject(pickList[0]);
			}
		}
	}
}

void Picking::PostUpdate()
{
}

void Picking::Render()
{
}

void Picking::PreRender()
{
}

void Picking::UIRender()
{
}
