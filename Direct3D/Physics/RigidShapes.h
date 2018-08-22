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

class ShapeTerrain : public IShapes
{
public:
	ShapeTerrain(UINT x, UINT y, void* pHeightData,float max,float min = 0.f)
		:IShapes(0.f)
	{
		if (pHeightData == nullptr)
		{
			float* temp = new float[x*y];
			for (int i = 0; i < y;i++)
			{
				for (int j = 0; j < x;j++)
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
//btshape TODO : Ä¸½¶, ½ºÇÇ¾î(Á¾·ù 4°¡Áö - xyz + default ), ½Ç¸°´õ, ¿ø»Ô

//class ShapeCapsule : public btCapsuleShape
//{
//public : 
//
//};
//
