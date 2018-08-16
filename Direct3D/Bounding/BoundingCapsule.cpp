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
	//1. 평행할때
	//점과 직선사이의 거리를 구한다. /-> 외적을 이용하여 구함 dist = |(a1 - b1) x dir2 | / dir2.length
	//거리가 반지름 합보다 작다면
	//점과 직선사이의 각도를 구한다음
	//90도 보다 작고, 선분의 길이의 제곱보다 작으면 충돌

	//2. 평행안할때
	//두 선분을 직선의 방정식으로 나타냄
	//최단점 p, q를 이은 벡터 Wc구함
	//Wc = p - q
	//Wc = a1 + s1 * dir1 - b1 + s2 * dir2
	//line1 = a1 + s1 * dir1
	//line2 = b1 + s2 * dir2
	//연립방정식으로 s1, s2 구함
	//s1와 s2가 선분밖에있으면 s1 이 1일때 0 일떄 s2가 1일때 0일때 각각 최단거리구함
	//최단거리가 반지름합보다 작으면 충돌아니면 충돌 x
	//s1와 s2가 선분 안에있다면
	//s1와 s2를 대입한 line의 지점 두개의 거리구함
	//거리가 반지름합보다 작으면 충돌 아니면 충돌x
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
			//두 선분의 끝점 4개를 각각 다른 선분과의 각도를 구하고
			//하나라도 90도보다 작고, 선분의 길이의 제곱보다 작으면 충돌
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
	else //평행하지 않을때
	{
		float s1, s2;

		D3DXVECTOR3 W0 = a1 - b1;
		D3DXVECTOR3 Wc = W0 + dir1 * s1 - dir2 * s2;

		//최단점 이은 벡터 Wc는 dir1, dir2 와 직교

		float a, b, c, d, e;

		a = D3DXVec3Dot(&dir1, &dir1);
		b = D3DXVec3Dot(&dir1, &dir2);
		c = D3DXVec3Dot(&dir2, &dir2);
		d = D3DXVec3Dot(&dir1, &W0);
		e = D3DXVec3Dot(&dir2, &W0);
	

		s1 = (b * e - c * d) / (a * c - b * b);
		s2 = (a * e - b * d) / (a * c - b * b);

		//선분밖일땐 양끝점과 거리를 구해야함
		float dist = Math::FloatMax;
		if (s1 < 0) //a1 과 line2 거리
		{
			D3DXVECTOR3 cross;
			D3DXVec3Cross(&cross, &(a1 - b1), &dir2);
			dist = Math::Abs(D3DXVec3Length(&cross) / D3DXVec3Length(&dir2));
		}
		else if (s1 > 1) //a2 과 line2 거리
		{
			D3DXVECTOR3 cross;
			D3DXVec3Cross(&cross, &(a2 - b1), &dir2);
			dist = Math::Abs(D3DXVec3Length(&cross) / D3DXVec3Length(&dir2));
		}
		else if (s2 < 0) //b1 과 line1 거리
		{
			D3DXVECTOR3 cross;
			D3DXVec3Cross(&cross, &(b1 - a1), &dir1);
			dist = Math::Abs(D3DXVec3Length(&cross) / D3DXVec3Length(&dir1));
		}
		else if (s2 > 1) //b2 과 line1 거리
		{
			D3DXVECTOR3 cross;
			D3DXVec3Cross(&cross, &(b2 - a1), &dir1);
			dist = Math::Abs(D3DXVec3Length(&cross) / D3DXVec3Length(&dir1));
		}
		else //최단점이 두선분에 있을때
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
