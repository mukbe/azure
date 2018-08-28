#include "stdafx.h"
#include "BoundingFrustum.h"
#include "Bounding.h"


BoundingFrustum::BoundingFrustum()
{
	vertex[0] = transformVertex[0] = D3DXVECTOR3(-1.f, 1.f, 0.f);
	vertex[1] = transformVertex[1] = D3DXVECTOR3(1.f, 1.f, 0.f);
	vertex[2] = transformVertex[2] = D3DXVECTOR3(-1.f, -1.f, 0.f);
	vertex[3] = transformVertex[3] = D3DXVECTOR3(1.f, -1.f, 0.f);

	vertex[4] = transformVertex[4] = D3DXVECTOR3(-1.f, 1.f, 1.f);
	vertex[5] = transformVertex[5] = D3DXVECTOR3(1.f, 1.f, 1.f);
	vertex[6] = transformVertex[6] = D3DXVECTOR3(-1.f, -1.f, 1.f);
	vertex[7] = transformVertex[7] = D3DXVECTOR3(1.f, -1.f, 1.f);

	ZeroMemory(plane, sizeof D3DXPLANE * 6);
}


BoundingFrustum::~BoundingFrustum()
{
}

void BoundingFrustum::CreatePlane(LPD3DXPLANE pOut, const D3DXVECTOR3 * v0, const D3DXVECTOR3 * v1, const D3DXVECTOR3 * v2)
{
	//����� �븻 
	D3DXVECTOR3 normal;
	D3DXVECTOR3 edge1 = *v1 - *v0;
	D3DXVECTOR3 edge2 = *v2 - *v0;

	D3DXVec3Cross(&normal, &edge1, &edge2);
	D3DXVec3Normalize(&normal, &normal);

	//����� �븻 �������� ���������� �ִ� �Ÿ�
	float dist = -D3DXVec3Dot(&normal, v0);

	pOut->a = normal.x;
	pOut->b = normal.y;
	pOut->c = normal.z;
	pOut->d = dist;
}

float BoundingFrustum::PlaneDot(const LPD3DXPLANE pPlane, const D3DXVECTOR3 * point)
{
	D3DXVECTOR3 normal(pPlane->a, pPlane->b, pPlane->c);

	////��鿡�� ������ ����
	//D3DXVECTOR3 p = normal * -pPlane->d;

	////������ ������ �������� ���� �Ű������� ���� �������� ���⺤��
	//D3DXVECTOR3 dir = *point - p;

	////����
	//float distance = D3DXVec3Dot(&normal, &dir);

	float distance = D3DXVec3Dot(&normal, point) + pPlane->d;

	return distance;
}

void BoundingFrustum::UpdateFrustum(D3DXMATRIX viewProj)
{
	D3DXMATRIX matInv;
	D3DXMatrixInverse(&matInv, NULL, &viewProj);

	for (UINT i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(transformVertex + i, vertex + i, &matInv);

	//����
	this->CreatePlane(&plane[0], &transformVertex[0], &transformVertex[1], &transformVertex[2]);
	//�ĸ�
	this->CreatePlane(&plane[1], &transformVertex[5], &transformVertex[4], &transformVertex[7]);
	//����
	this->CreatePlane(&plane[2], &transformVertex[1], &transformVertex[5], &transformVertex[3]);
	//����
	this->CreatePlane(&plane[3], &transformVertex[4], &transformVertex[0], &transformVertex[6]);
	//���
	this->CreatePlane(&plane[4], &transformVertex[4], &transformVertex[5], &transformVertex[0]);
	//�ϴ�
	this->CreatePlane(&plane[5], &transformVertex[2], &transformVertex[3], &transformVertex[6]);
}

void BoundingFrustum::Render()
{
	D3DXCOLOR color = D3DXCOLOR(0.f, 0.f, 1.f, 1.f);
	for (int i = 0; i < 3; i++)
	{
		GizmoRenderer->Line(transformVertex[i], transformVertex[i + 1], color);
		GizmoRenderer->Line(transformVertex[i + 4], transformVertex[i + 5], color);
		GizmoRenderer->Line(transformVertex[i], transformVertex[i + 4], color);
	}
	GizmoRenderer->Line(transformVertex[0], transformVertex[3], color);
	GizmoRenderer->Line(transformVertex[4], transformVertex[7], color);
	GizmoRenderer->Line(transformVertex[3], transformVertex[7], color);
}


bool BoundingFrustum::IsPointInFrustum(D3DXVECTOR3 * p)
{
	float fDist;
	{
		fDist = D3DXPlaneDotCoord(&plane[3], p);
		if (fDist > PLANE_EPSILON) return false;	// plane�� normal���Ͱ� far�� ���ϰ� �����Ƿ� ����̸� ���������� �ٱ���
		fDist = D3DXPlaneDotCoord(&plane[0], p);
		if (fDist > PLANE_EPSILON) return false;	// plane�� normal���Ͱ� left�� ���ϰ� �����Ƿ� ����̸� ���������� ����
		fDist = D3DXPlaneDotCoord(&plane[2], p);
		if (fDist > PLANE_EPSILON) return false;	// plane�� normal���Ͱ� right�� ���ϰ� �����Ƿ� ����̸� ���������� ������
		fDist = D3DXPlaneDotCoord(&plane[4], p);
		if (fDist > PLANE_EPSILON)return false;
		fDist = D3DXPlaneDotCoord(&plane[5], p);
		if (fDist > PLANE_EPSILON)return false;
	}
	return true;
}

bool BoundingFrustum::IsSphereInFrustum(D3DXVECTOR3 * p, float radius)
{
	for (int i = 1; i < 6; i++)
	{
		float dist = this->PlaneDot(&plane[i], p);
		if (dist > radius)return false;
	}

	return true;
}
