#pragma once
#include "IShapes.h"
#include <src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

class ShapeBox : public IShapes
{
public:
	ShapeBox(D3DXVECTOR3 size = D3DXVECTOR3(1,1,1) , float m = 1.f)
		:IShapes(m)
	{
		memcpy(&(this->size), &size, sizeof(D3DXVECTOR3));

		shape = static_cast<btCollisionShape*>(new btBoxShape(this->size));
	}

private:
	btVector3 size;
};

class ShapePlane : public IShapes
{
public:
	ShapePlane(D3DXVECTOR3 normal = D3DXVECTOR3(0,1,0), float planeConstant = 0.f, float m = 0.f)
		:IShapes(m)
	{
		memcpy(&(this->normal), &normal, sizeof(D3DXVECTOR3));
		memcpy(&(this->d), &planeConstant, sizeof(float));

		shape = static_cast<btCollisionShape*>(new btStaticPlaneShape(this->normal, d));
	}
	
private:
	btVector3 normal;
	btScalar d;
};

//TODO 지금은 임시생성
class ShapeTerrain : public IShapes
{
public:
	ShapeTerrain(UINT x, UINT y, void* pHeightData,float max,float min = 0.f)
		:IShapes(0.f)
	{
		if (pHeightData == nullptr)
		{
			float* temp = new float[x*y];
			for (UINT i = 0; i < y;i++)
			{
				for (UINT j = 0; j < x;j++)
				{
					int index = i * y + j;
					temp[index] = 0.1f;
				}
			}

			shape = static_cast<btCollisionShape*>(new btHeightfieldTerrainShape(x, y, temp, max, 1, 1, 1));
		}
	}

private:
	btVector3 normal;
	btScalar d;
};
//btshape TODO : 캡슐, 스피어(종류 4가지 - xyz + default ), 실린더, 원뿔

//class ShapeCapsule : public btCapsuleShape
//{
//public : 
//
//};
//
