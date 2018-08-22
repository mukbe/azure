#pragma once
#include "stdafx.h"

class IShapes
{
public:
	IShapes(float m = 0.f)
	{
		trans.setIdentity();
		memcpy(&position, &trans.getOrigin(), sizeof(D3DXVECTOR4));

		mass = m;
	}
	void SetPosition(D3DXVECTOR3 pos)
	{ 
		memcpy(&position, &pos, sizeof(D3DXVECTOR3));
		position.w = 1.0f;
		trans.setOrigin(btVector3(pos.x, pos.y, pos.z));
	}
	void SetRotation(D3DXVECTOR3 rotation)
	{
		D3DXMATRIX rot;
		D3DXQUATERNION q;
		D3DXMatrixIdentity(&rot);
		D3DXQuaternionRotationYawPitchRoll(&q, rotation.y, rotation.x, rotation.z);
		D3DXMatrixRotationQuaternion(&rot, &q);

		memcpy(&trans, &rot, sizeof(D3DXMATRIX));
		memcpy(&trans.getOrigin(), &position, sizeof(D3DXVECTOR4));
	}
	void SetMass(float m) { mass = btScalar(m); }

	btCollisionShape* GetPointer() { return shape; }
	btScalar GetMass() { return mass; }
	btTransform GetTrans() { return trans; }

protected:
	btCollisionShape * shape;
	btTransform trans;
	btScalar mass;
	
	D3DXVECTOR4 position;
};