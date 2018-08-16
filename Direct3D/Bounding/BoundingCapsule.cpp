#include "stdafx.h"
#include "BoundingCapsule.h"



BoundingCapsule::BoundingCapsule()
	:center(0.f,0.f,0.f),direction(0.f,0.f,0.f),height(0.f),radius(0.f)
{
}

BoundingCapsule::BoundingCapsule(D3DXVECTOR3 center, D3DXVECTOR3 direction, float height, float radius)
	:center(center),direction(direction),height(height),radius(radius)
{
}

bool BoundingCapsule::Intersects(const BoundingCapsule & capsule) const
{
	//1. �����Ҷ�
	//���� ���������� �Ÿ��� ���Ѵ�. /-> ������ �̿��Ͽ� ���� dist = |(a1 - b1) x dir2 | / dir2.length
	//�Ÿ��� ������ �պ��� �۴ٸ�
	//���� ���������� ������ ���Ѵ���
	//90�� ���� �۰�, ������ ������ �������� ������ �浹

	//2. ������Ҷ�
	//�� ������ ������ ���������� ��Ÿ��
	//�ִ��� p, q�� ���� ���� Wc����
	//Wc = p - q
	//Wc = a1 + s1 * dir1 - b1 + s2 * dir2
	//line1 = a1 + s1 * dir1
	//line2 = b1 + s2 * dir2
	//�������������� s1, s2 ����
	//s1�� s2�� ���йۿ������� s1 �� 1�϶� 0 �ϋ� s2�� 1�϶� 0�϶� ���� �ִܰŸ�����
	//�ִܰŸ��� �������պ��� ������ �浹�ƴϸ� �浹 x
	//s1�� s2�� ���� �ȿ��ִٸ�
	//s1�� s2�� ������ line�� ���� �ΰ��� �Ÿ�����
	//�Ÿ��� �������պ��� ������ �浹 �ƴϸ� �浹x
	D3DXVECTOR3 dir1 = this->direction;
	D3DXVECTOR3 dir2 = capsule.direction;

	D3DXVECTOR3 a1, a2, b1, b2;
	float r1, r2;

	r1 = this->radius;
	r2 = capsule.radius;

	a1 = this->center - this->direction * (this->height / 2 - r1);
	a2 = this->center + this->direction * (this->height / 2 - r1);

	b1 = capsule.center - capsule.direction * (capsule.height / 2 - r2);
	b2 = capsule.center + capsule.direction * (capsule.height / 2 - r2);

	if (dir1 == dir2 || dir1 == -dir2)
	{
		D3DXVECTOR3 crossing;
		D3DXVec3Cross(&crossing, &(a1 - b1), &dir2);
		
		float dist = Math::Abs(D3DXVec3Length(&crossing) / D3DXVec3Length(&dir2));

		if (dist < r1 + r2)
		{
			//�� ������ ���� 4���� ���� �ٸ� ���а��� ������ ���ϰ�
			//�ϳ��� 90������ �۰�, ������ ������ �������� ������ �浹
			D3DXVECTOR3 line1 = a2 - a1;
			D3DXVECTOR3 line2 = b2 - b1;

			D3DXVECTOR3 a1ToLine2 = a1 - line2;
			D3DXVECTOR3 a2ToLine2 = a2 - line2;
			D3DXVECTOR3 b1ToLine1 = b1 - line1;
			D3DXVECTOR3 b2ToLine1 = b2 - line1;

			float length1 = D3DXVec3Length(&line1);
			float length2 = D3DXVec3Length(&line2);

			bool bCheck = false;
			bCheck |= CheckBetweenLines(line1, b1ToLine1, length1);
			bCheck |= CheckBetweenLines(line1, b2ToLine1, length1);
			bCheck |= CheckBetweenLines(line2, a1ToLine2, length2);
			bCheck |= CheckBetweenLines(line2, a2ToLine2, length2);

			if (bCheck == true) return true;
		}
		else
			return false;
	}
	else //�������� ������
	{
		float s1, s2;

		D3DXVECTOR3 W0 = a1 - b1;
		D3DXVECTOR3 Wc = W0 + dir1 * s1 - dir2 * s2;

		//�ִ��� ���� ���� Wc�� dir1, dir2 �� ����

		float a, b, c, d, e;

		a = D3DXVec3Dot(&dir1, &dir1);
		b = D3DXVec3Dot(&dir1, &dir2);
		c = D3DXVec3Dot(&dir2, &dir2);
		d = D3DXVec3Dot(&dir1, &W0);
		e = D3DXVec3Dot(&dir2, &W0);
	

		s1 = (b * e - c * d) / (a * c - b * b);
		s2 = (a * e - b * d) / (a * c - b * b);

		//���й��϶� �糡���� �Ÿ��� ���ؾ���
		float dist = Math::FloatMax;
		if (s1 < 0) //a1 �� line2 �Ÿ�
		{
			D3DXVECTOR3 cross;
			D3DXVec3Cross(&cross, &(a1 - b1), &dir2);
			dist = Math::Abs(D3DXVec3Length(&cross) / D3DXVec3Length(&dir2));
		}
		else if (s1 > 1) //a2 �� line2 �Ÿ�
		{
			D3DXVECTOR3 cross;
			D3DXVec3Cross(&cross, &(a2 - b1), &dir2);
			dist = Math::Abs(D3DXVec3Length(&cross) / D3DXVec3Length(&dir2));
		}
		else if (s2 < 0) //b1 �� line1 �Ÿ�
		{
			D3DXVECTOR3 cross;
			D3DXVec3Cross(&cross, &(b1 - a1), &dir1);
			dist = Math::Abs(D3DXVec3Length(&cross) / D3DXVec3Length(&dir1));
		}
		else if (s2 > 1) //b2 �� line1 �Ÿ�
		{
			D3DXVECTOR3 cross;
			D3DXVec3Cross(&cross, &(b2 - a1), &dir1);
			dist = Math::Abs(D3DXVec3Length(&cross) / D3DXVec3Length(&dir1));
		}
		else //�ִ����� �μ��п� ������
		{
			D3DXVECTOR3 p1 = a1 + dir1 * s1;
			D3DXVECTOR3 p2 = b1 + dir2 * s2;
			float length = D3DXVec3Length(&(p1 - p2));

			if (length < r1 + r2) return true;
		}
		if (dist < r1 + r2) return true;
	}
	return false;
}

bool BoundingCapsule::CheckBetweenLines(const D3DXVECTOR3 & line1, const D3DXVECTOR3 & line2, float length) const
{
	
	if (Math::Angle(line1, line2) <  1.570796f)		//PiOver2 ==  1.570796f
	{
		float dot = D3DXVec3Dot(&line1, &line2);

		if (dot < length * length)
		{
			return true;
		}
	}
	return false;
}


BoundingCapsule BoundingCapsule::Transform(D3DXMATRIX mat)
{
	return BoundingCapsule();
}
