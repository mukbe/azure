#include "stdafx.h"
#include "BoundingBox.h"

#include "Ray.h"
#include "Bounding.h"
#include "BoundingSphere.h"
#include "BoundingCapsule.h"
#include "BoundingFrustum.h"
#include "../Utilities/Math.h"

#include "../Utilities/Transform.h"


BoundingBox::BoundingBox()
	:minPos(0.f,0.f,0.f),maxPos(0.f,0.f,0.f),halfSize(0.f,0.f,0.f)
{
}

BoundingBox::BoundingBox(D3DXVECTOR3 min, D3DXVECTOR3 max)
	:minPos(min),maxPos(max)
{
	D3DXVECTOR3 center = (minPos + maxPos) / 2.0f;

	this->halfSize = center - minPos;
}

BoundingBox::BoundingBox(BoundingSphere sphere)
{
	this->minPos.x = sphere.center.x - sphere.radius;
	this->minPos.y = sphere.center.y - sphere.radius;
	this->minPos.z = sphere.center.z - sphere.radius;
	this->maxPos.x = sphere.center.x + sphere.radius;
	this->maxPos.y = sphere.center.y + sphere.radius;
	this->maxPos.z = sphere.center.z + sphere.radius;

	this->halfSize = sphere.center - minPos;
}

void BoundingBox::Render(D3DXMATRIX matWorld,bool drawAABB /*=true*/,D3DXCOLOR color /*=Red*/)
{
	//   0-------1
	//  /|      /|
	// 4-------5 |
	// | 3-----|-2
	// |/      |/
	// 7-------6 
	vector<D3DXVECTOR3> corners;
	this->GetCorners(corners, matWorld);

	GizmoRenderer->OBB(corners, color);

	if (drawAABB)
	{
		D3DXVECTOR3 min, max;
		D3DXComputeBoundingBox(&corners[0], corners.size(), sizeof D3DXVECTOR3, &min, &max);
		GizmoRenderer->AABB(min, max, D3DXCOLOR(0.f, 1.f, 0.f, 1.f));
	}
	
}

void BoundingBox::GetCorners(D3DXVECTOR3 * pOut)
{
	//   0-------1
	//  /|      /|
	// 4-------5 |
	// | 3-----|-2
	// |/      |/
	// 7-------6 
	pOut[0] = D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z);
	pOut[1] = D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z);
	pOut[2] = D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z);
	pOut[3] = D3DXVECTOR3(minPos.x, minPos.y, maxPos.z);
	pOut[4] = D3DXVECTOR3(minPos.x, maxPos.y, minPos.z);
	pOut[5] = D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z);
	pOut[6] = D3DXVECTOR3(maxPos.x, minPos.y, minPos.z);
	pOut[7] = D3DXVECTOR3(minPos.x, minPos.y, minPos.z);
}

void BoundingBox::GetCorners(vector<D3DXVECTOR3>& pOut)
{

	//   0-------1
	//  /|      /|
	// 4-------5 |
	// | 3-----|-2
	// |/      |/
	// 7-------6 
	if (pOut.empty() == false)
		pOut.clear();

	pOut.assign(8, D3DXVECTOR3());

	pOut[0] = D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z);
	pOut[1] = D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z);
	pOut[2] = D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z);
	pOut[3] = D3DXVECTOR3(minPos.x, minPos.y, maxPos.z);
	pOut[4] = D3DXVECTOR3(minPos.x, maxPos.y, minPos.z);
	pOut[5] = D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z);
	pOut[6] = D3DXVECTOR3(maxPos.x, minPos.y, minPos.z);
	pOut[7] = D3DXVECTOR3(minPos.x, minPos.y, minPos.z);
}

void BoundingBox::GetCorners(vector<D3DXVECTOR3>& pOut, D3DXMATRIX mat)
{
	//   0-------1
	//  /|      /|
	// 4-------5 |
	// | 3-----|-2
	// |/      |/
	// 7-------6 

	if (pOut.empty() == false)
		pOut.clear();

	D3DXVECTOR3 currentVertex;
	D3DXVec3TransformCoord(&currentVertex, &D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z), &mat);
	pOut.push_back(currentVertex);
	D3DXVec3TransformCoord(&currentVertex, &D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z), &mat);
	pOut.push_back(currentVertex);
	D3DXVec3TransformCoord(&currentVertex, &D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z), &mat);
	pOut.push_back(currentVertex);
	D3DXVec3TransformCoord(&currentVertex, &D3DXVECTOR3(minPos.x, minPos.y, maxPos.z), &mat);
	pOut.push_back(currentVertex);

	D3DXVec3TransformCoord(&currentVertex, &D3DXVECTOR3(minPos.x, maxPos.y, minPos.z), &mat);
	pOut.push_back(currentVertex);
	D3DXVec3TransformCoord(&currentVertex, &D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z), &mat);
	pOut.push_back(currentVertex);
	D3DXVec3TransformCoord(&currentVertex, &D3DXVECTOR3(maxPos.x, minPos.y, minPos.z), &mat);
	pOut.push_back(currentVertex);
	D3DXVec3TransformCoord(&currentVertex, &D3DXVECTOR3(minPos.x, minPos.y, minPos.z), &mat);
	pOut.push_back(currentVertex);
}

bool BoundingBox::IntersectsAABB(BoundingBox box)
{
	if ((double)maxPos.x < (double)box.minPos.x || (double)minPos.x >(double)box.maxPos.x || ((double)maxPos.y < (double)box.minPos.y
		|| (double)minPos.y >(double)box.maxPos.y) || ((double)maxPos.z < (double)box.minPos.z || (double)minPos.z >(double)box.maxPos.z))
		return false;
	else
		return true;
}

bool BoundingBox::Intersects(BoundingFrustum frustum)
{
	return false;
}

PlaneIntersectionType BoundingBox::Intersects(D3DXPLANE plane)
{
	return PlaneIntersectionType_Front;

}

bool BoundingBox::Intersects(Ray ray, float * result)
{
	float num1 = 0.0f;
	float num2 = Math::FloatMax;


	//1. ray.Direction.X
	if ((double)Math::Abs(ray.direction.x) < 9.99999997475243E-07)
	{
		if ((double)ray.origin.x < (double)minPos.x || (double)ray.origin.x >(double)maxPos.x)
			return false;
	}
	else
	{
		float num3 = 1.0f / ray.direction.x;
		float num4 = (minPos.x - ray.origin.x)* num3;
		float num5 = (maxPos.x - ray.origin.x)* num3;

		if ((double)num4 > (double)num5)
		{
			float num6 = num4;
			num4 = num5;
			num5 = num6;
		}

		num1 = Math::Max(num4, num1);
		num2 = Math::Min(num5, num2);

		if ((double)num1 > (double)num2)
			return false;
	}


	//2. ray.Direction.Y
	if ((double)Math::Abs(ray.direction.y) < 9.99999997475243E-07)
	{
		if ((double)ray.origin.y < (double)minPos.y || (double)ray.origin.y >(double)maxPos.y)
			return false;
	}
	else
	{
		float num3 = 1.0f / ray.direction.y;
		float num4 = (minPos.y - ray.origin.y) * num3;
		float num5 = (maxPos.y - ray.origin.y) * num3;

		if ((double)num4 > (double)num5)
		{
			float num6 = num4;
			num4 = num5;
			num5 = num6;
		}

		num1 = Math::Max(num4, num1);
		num2 = Math::Min(num5, num2);

		if ((double)num1 > (double)num2)
			return false;
	}


	//3. ray.Direction.Z
	if ((double)Math::Abs(ray.direction.z) < 9.99999997475243E-07)
	{
		if ((double)ray.origin.z < (double)minPos.z || (double)ray.origin.z >(double)maxPos.z)
			return false;
	}
	else
	{
		float num3 = 1.0f / ray.direction.z;
		float num4 = (minPos.z - ray.origin.z) * num3;
		float num5 = (maxPos.z - ray.origin.z) * num3;

		if ((double)num4 > (double)num5)
		{
			float num6 = num4;
			num4 = num5;
			num5 = num6;
		}

		num1 = Math::Max(num4, num1);
		float num7 = Math::Min(num5, num2);

		if ((double)num1 > (double)num7)
			return false;
	}

	*result = num1;
	return true;
}

bool BoundingBox::Intersects(BoundingSphere sphere)
{
	D3DXVECTOR3 result1;
	result1.x = Math::Clamp(sphere.center.x, minPos.x, maxPos.x);
	result1.y = Math::Clamp(sphere.center.y, minPos.y, maxPos.y);
	result1.z = Math::Clamp(sphere.center.z, minPos.z, maxPos.z);

	float result2 = D3DXVec3LengthSq(&(sphere.center - result1));// Vector3::DistanceSquared(sphere.Center, result1);

	if ((double)result2 <= (double)sphere.radius * (double)sphere.radius)
		return true;
	else
		return false;
}

bool BoundingBox::IntersectsOBB(Transform * pTransA, BoundingBox * pBoundA, Transform * pTransB, BoundingBox * pBoundB)
{
	//�迭����
	//X = 0, Y = 1, Z = 2;
	//OBB �浹�� �ʿ��� �浹 ����ü
	struct OBB {
		D3DXVECTOR3 center;		//�߽���
		D3DXVECTOR3 axis[3];	//�����	
		float halfLength[3];	//���࿡ ���� ���� ũ��
	};

	D3DXVECTOR3 centerA, centerB;
	D3DXVec3TransformCoord(&centerA, &(pBoundA->minPos + pBoundA->halfSize), &pTransA->GetFinalMatrix());
	D3DXVec3TransformCoord(&centerB, &(pBoundB->minPos + pBoundB->halfSize), &pTransB->GetFinalMatrix());

	//
	// A �ٿ���� ���� �浹 ����ü
	//
	OBB obbA;

	//�� �� ����
	obbA.axis[0] = pTransA->GetRight();
	obbA.axis[1] = pTransA->GetUp();
	obbA.axis[2] = pTransA->GetForward();

	//����
	obbA.center = centerA;

	//����������
	D3DXVECTOR3 scaleA = pTransA->GetScale();
	obbA.halfLength[0] = pBoundA->halfSize.x * scaleA.x;
	obbA.halfLength[1] = pBoundA->halfSize.y * scaleA.y;
	obbA.halfLength[2] = pBoundA->halfSize.z * scaleA.z;

	//
	// B �ٿ���� ���� �浹 ����ü
	//
	OBB obbB;

	//�� �� ����
	obbB.axis[0] = pTransB->GetRight();
	obbB.axis[1] = pTransB->GetUp();
	obbB.axis[2] = pTransB->GetForward();

	//����
	obbB.center = centerB;

	//����������
	D3DXVECTOR3 scaleB = pTransB->GetScale();
	obbB.halfLength[0] = pBoundB->halfSize.x * scaleB.x;
	obbB.halfLength[1] = pBoundB->halfSize.y * scaleB.y;
	obbB.halfLength[2] = pBoundB->halfSize.z * scaleB.z;


	//
	// OBB �浹
	//
	float cos[3][3];				//���� �������� ���� �ڻ��� ��  [A��][B��]  ( [0][1] => ���ε����� A�� X ��� B�� Y ���� ������ ���� cos ���̴� )
	float absCos[3][3];				//���� �������� ���� �ڻ��� ���밪  [A��][B��]  ( [0][1] => ���ε����� A�� X ��� B�� Y ���� ������ ���� cos ���� ���̴� )
	float dist[3];					//A �ٿ�� �������� A �߽������� B �� �߽������͸� ������ ��������			


	const float cutOff = 0.99999f;		//���� �Ǵ��� �ϱ����� �ƿ��� �� ( ��� �� ���� cos ��� ���� �̺��� ũ�ٸ� �� �浹ü�� ������ �����ϴٴ� ���� )
	bool existParallelPair = false;		//�� ���̶� �����ϳ�?

										//A ���� B �� ���⺤��
	D3DXVECTOR3 D = obbB.center - obbA.center;

	float r, r0, r1;			//r0 �� r1 �� ���� r ���� ������ �浹 ���� 

	for (int a = 0; a < 3; a++)
	{
		for (int b = 0; b < 3; b++)
		{
			cos[a][b] = D3DXVec3Dot(&obbA.axis[a], &obbB.axis[b]);
			absCos[a][b] = abs(cos[a][b]);

			//������ ���� ���� �Ǵ� ��Ȯ��
			if (absCos[a][b] > cutOff) existParallelPair = true;
		}

		//���ͳ����� ���⺤�͸� A �ٿ�� Axis �� ������ �Ÿ�
		dist[a] = D3DXVec3Dot(&obbA.axis[a], &D);
	}


	//
	// A �ٿ�� �ڽ��� X ���� �������� �� ����
	//

	//r �� dist[0] �� ���� ���� �ȴ�.
	r = abs(dist[0]);

	//r0 
	r0 = obbA.halfLength[0];

	r1 = absCos[0][0] * obbB.halfLength[0] +
		absCos[0][1] * obbB.halfLength[1] +
		absCos[0][2] * obbB.halfLength[2];

	if (r > r0 + r1) return false;


	//
	// A �ٿ�� �ڽ��� Y ���� �������� �� ����
	//

	//r �� dist[1] �� ���� ���� �ȴ�.
	r = abs(dist[1]);

	//r0 
	r0 = obbA.halfLength[1];

	//r1 
	r1 = absCos[1][0] * obbB.halfLength[0] +
		absCos[1][1] * obbB.halfLength[1] +
		absCos[1][2] * obbB.halfLength[2];

	if (r > r0 + r1) return false;

	//
	// A �ٿ�� �ڽ��� Z ���� �������� �� ����
	//

	//r �� dist[2] �� ���� ���� �ȴ�.
	r = abs(dist[2]);

	//r0 
	r0 = obbA.halfLength[2];

	//r1 
	r1 = absCos[2][0] * obbB.halfLength[0] +
		absCos[2][1] * obbB.halfLength[1] +
		absCos[2][2] * obbB.halfLength[2];

	if (r > r0 + r1) return false;




	//
	// B �ٿ�� �ڽ��� X ���� �������� �� ����
	//
	r = abs(D3DXVec3Dot(&obbB.axis[0], &D));

	//r0 
	r0 = absCos[0][0] * obbA.halfLength[0] +
		absCos[1][0] * obbA.halfLength[1] +
		absCos[2][0] * obbA.halfLength[2];

	//r1 
	r1 = obbB.halfLength[0];

	if (r > r0 + r1) return false;

	//
	// B �ٿ�� �ڽ��� Y ���� �������� �� ����
	//
	r = abs(D3DXVec3Dot(&obbB.axis[1], &D));

	//r0 
	r0 = absCos[0][1] * obbA.halfLength[0] +
		absCos[1][1] * obbA.halfLength[1] +
		absCos[2][1] * obbA.halfLength[2];

	//r1 
	r1 = obbB.halfLength[1];
	if (r > r0 + r1) return false;

	//
	// B �ٿ�� �ڽ��� Z ���� �������� �� ����
	//
	r = abs(D3DXVec3Dot(&obbB.axis[2], &D));

	//r0 
	r0 = absCos[0][2] * obbA.halfLength[0] +
		absCos[1][2] * obbA.halfLength[1] +
		absCos[2][2] * obbA.halfLength[2];

	//r1 
	r1 = obbB.halfLength[2];
	if (r > r0 + r1) return false;


	//�����Դµ� ���е��� �ʾҴ�.. �׷��� existParallelPair true �̸�
	//������ �����ϴٴ� �����ε� �̷��� �и��� 6 ���� �˻��ϸ�ȴ�....
	if (existParallelPair) return true;


	//////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////

	//A�� X �� B �� X �࿡ ���� ������ �浹 üũ
	r = abs(dist[2] * cos[1][0] - dist[1] * cos[2][0]);
	r0 = obbA.halfLength[1] * absCos[2][0] + obbA.halfLength[2] * absCos[1][0];
	r1 = obbB.halfLength[1] * absCos[0][2] + obbB.halfLength[2] * absCos[0][1];
	if (r > r0 + r1)
		return false;


	//A�� X �� B �� Y �࿡ ���� ������ �浹 üũ
	r = abs(dist[2] * cos[1][1] - dist[1] * cos[2][1]);
	r0 = obbA.halfLength[1] * absCos[2][1] + obbA.halfLength[2] * absCos[1][1];
	r1 = obbB.halfLength[0] * absCos[0][2] + obbB.halfLength[2] * absCos[0][0];
	if (r > r0 + r1)
		return false;


	//A�� X �� B �� Z �࿡ ���� ������ �浹 üũ
	r = abs(dist[2] * cos[1][2] - dist[1] * cos[2][2]);
	r0 = obbA.halfLength[1] * absCos[2][2] + obbA.halfLength[2] * absCos[1][2];
	r1 = obbB.halfLength[0] * absCos[0][1] + obbB.halfLength[1] * absCos[0][0];
	if (r > r0 + r1)
		return false;

	/////////////////////////////////////////////////////////////////

	//A�� Y �� B �� X �࿡ ���� ������ �浹 üũ
	r = abs(dist[0] * cos[2][0] - dist[2] * cos[0][0]);
	r0 = obbA.halfLength[0] * absCos[2][0] + obbA.halfLength[2] * absCos[0][0];
	r1 = obbB.halfLength[1] * absCos[1][2] + obbB.halfLength[2] * absCos[1][1];
	if (r > r0 + r1)
		return false;

	//A�� Y �� B �� Y �࿡ ���� ������ �浹 üũ
	r = abs(dist[0] * cos[2][1] - dist[2] * cos[0][1]);
	r0 = obbA.halfLength[0] * absCos[2][1] + obbA.halfLength[2] * absCos[0][1];
	r1 = obbB.halfLength[0] * absCos[1][2] + obbB.halfLength[2] * absCos[1][0];
	if (r > r0 + r1)
		return false;

	//A�� Y �� B �� Z �࿡ ���� ������ �浹 üũ
	r = abs(dist[0] * cos[2][2] - dist[2] * cos[0][2]);
	r0 = obbA.halfLength[0] * absCos[2][2] + obbA.halfLength[2] * absCos[0][2];
	r1 = obbB.halfLength[0] * absCos[1][1] + obbB.halfLength[1] * absCos[1][0];
	if (r > r0 + r1)
		return false;


	/////////////////////////////////////////////////////////////////	 

	//A�� Z �� B �� X �࿡ ���� ������ �浹 üũ
	r = abs(dist[1] * cos[0][0] - dist[0] * cos[1][0]);
	r0 = obbA.halfLength[0] * absCos[1][0] + obbA.halfLength[1] * absCos[0][0];
	r1 = obbB.halfLength[1] * absCos[2][2] + obbB.halfLength[2] * absCos[2][1];
	if (r > r0 + r1)
		return false;

	//A�� Z �� B �� Y �࿡ ���� ������ �浹 üũ
	r = abs(dist[1] * cos[0][1] - dist[0] * cos[1][1]);
	r0 = obbA.halfLength[0] * absCos[1][1] + obbA.halfLength[1] * absCos[0][1];
	r1 = obbB.halfLength[0] * absCos[2][2] + obbB.halfLength[2] * absCos[2][0];
	if (r > r0 + r1)
		return false;

	//A�� Z �� B �� Z �࿡ ���� ������ �浹 üũ
	r = abs(dist[1] * cos[0][2] - dist[0] * cos[1][2]);
	r0 = obbA.halfLength[0] * absCos[1][2] + obbA.halfLength[1] * absCos[0][2];
	r1 = obbB.halfLength[0] * absCos[2][1] + obbB.halfLength[1] * absCos[2][0];
	if (r > r0 + r1)
		return false;

	return true;
}



