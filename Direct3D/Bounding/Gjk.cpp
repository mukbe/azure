#include "stdafx.h"
#include "Gjk.h"

#include "./Utilities/Math.h"

//////////////////////////////////////////////////////////////////////////

const int Gjk::BitsToIndices[16] = { 0, 1, 2, 17, 3, 25, 26, 209, 4, 33, 34, 273, 35, 281, 282, 2257 };

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 생성자
//////////////////////////////////////////////////////////////////////////
Gjk::Gjk(void)
{

}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 심플렉스가 가득찼는지 여부
///@return 결과
//////////////////////////////////////////////////////////////////////////
bool Gjk::FullSimplex() const
{
	return simplexBits == 15;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 제곱으로 계산한 크기 중 가장 큰 값
///@return 결과 값
//////////////////////////////////////////////////////////////////////////
float Gjk::MaxLengthSquared() const
{
	return maxLengthSq;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 가까운 위치 벡터
///@return 결과 벡터
//////////////////////////////////////////////////////////////////////////
D3DXVECTOR3 Gjk::ClosestPoint() const
{
	return closestPoint;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 리셋
//////////////////////////////////////////////////////////////////////////
void Gjk::Reset()
{
	simplexBits = 0;
	maxLengthSq = 0.0f;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 검사할 포인트 추가
///@return 추가 결과
//////////////////////////////////////////////////////////////////////////
bool Gjk::AddSupportPoint(D3DXVECTOR3 newPoint)
{
	int index1 = (BitsToIndices[simplexBits ^ 15] & 7) - 1;
	y[index1] = newPoint;
	yLengthSq[index1] = this->LengthSquared(newPoint);

	int num = BitsToIndices[simplexBits];
	while (num != 0)
	{
		int index2 = (num & 7) - 1;
		D3DXVECTOR3 vector3 = y[index2] - newPoint;

		edges[index2][index1] = vector3;
		edges[index1][index2] = -vector3;
		edgeLengthSq[index1][index2] = edgeLengthSq[index2][index1] = this->LengthSquared(vector3);
		num >>= 3;
	}
	UpdateDeterminant(index1);

	return UpdateSimplex(index1);
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 행렬식 업데이트
///@param xmIdx : 갱신할 Determinant 번호
//////////////////////////////////////////////////////////////////////////
void Gjk::UpdateDeterminant(int index)
{
	int index1 = 1 << index;
	det[index1][index] = 1.0f;

	int num1 = Gjk::BitsToIndices[simplexBits];
	int num2 = num1;
	int num3 = 0;
	while (num2 != 0)
	{
		int index2 = (num2 & 7) - 1;
		int num4 = 1 << index2;
		int index3 = num4 | index1;
		det[index3][index2] = Dot(edges[index][index2], y[index]);
		det[index3][index] = Dot(edges[index2][index], y[index2]);

		int num5 = num1;
		for (int index4 = 0; index4 < num3; ++index4)
		{
			int index5 = (num5 & 7) - 1;
			int num6 = 1 << index5;
			int index6 = index3 | num6;

			int index7 = (double)edgeLengthSq[index2][index5] < (double)edgeLengthSq[index][index5] ? index2 : index;
			det[index6][index5] = (float)((double)det[index3][index2] * (double)Dot(edges[index7][index5], y[index2]) + (double)det[index3][index] * (double)Dot(edges[index7][index5], y[index]));

			int index8 = (double)edgeLengthSq[index5][index2] < (double)edgeLengthSq[index][index2] ? index5 : index;
			det[index6][index2] = (float)((double)det[num6 | index1][index5] * (double)Dot(edges[index8][index2], y[index5]) + (double)det[num6 | index1][index] * (double)Dot(edges[index8][index2], y[index]));

			int index9 = (double)edgeLengthSq[index2][index] < (double)edgeLengthSq[index5][index] ? index2 : index5;
			det[index6][index] = (float)((double)det[num4 | num6][index5] * (double)Dot(edges[index9][index], y[index5]) + (double)det[num4 | num6][index2] * (double)Dot(edges[index9][index], y[index2]));

			num5 >>= 3;
		}

		num2 >>= 3;
		++num3;
	}

	if ((simplexBits | index1) == 15)
	{
		int index2 = (double)edgeLengthSq[1][0] < (double)edgeLengthSq[2][0] ? ((double)edgeLengthSq[1][0] < (double)edgeLengthSq[3][0] ? 1 : 3) : ((double)edgeLengthSq[2][0] < (double)edgeLengthSq[3][0] ? 2 : 3);
		det[15][0] = (float)((double)det[14][1] * (double)Dot(edges[index2][0], y[1]) + (double)det[14][2] * (double)Dot(edges[index2][0], y[2]) + (double)det[14][3] * (double)Dot(edges[index2][0], y[3]));

		int index3 = (double)edgeLengthSq[0][1] < (double)edgeLengthSq[2][1] ? ((double)edgeLengthSq[0][1] < (double)edgeLengthSq[3][1] ? 0 : 3) : ((double)edgeLengthSq[2][1] < (double)edgeLengthSq[3][1] ? 2 : 3);
		det[15][1] = (float)((double)det[13][0] * (double)Dot(edges[index3][1], y[0]) + (double)det[13][2] * (double)Dot(edges[index3][1], y[2]) + (double)det[13][3] * (double)Dot(edges[index3][1], y[3]));

		int index4 = (double)edgeLengthSq[0][2] < (double)edgeLengthSq[1][2] ? ((double)edgeLengthSq[0][2] < (double)edgeLengthSq[3][2] ? 0 : 3) : ((double)edgeLengthSq[1][2] < (double)edgeLengthSq[3][2] ? 1 : 3);
		det[15][2] = (float)((double)det[11][0] * (double)Dot(edges[index4][2], y[0]) + (double)det[11][1] * (double)Dot(edges[index4][2], y[1]) + (double)det[11][3] * (double)Dot(edges[index4][2], y[3]));

		int index5 = (double)edgeLengthSq[0][3] < (double)edgeLengthSq[1][3] ? ((double)edgeLengthSq[0][3] < (double)edgeLengthSq[2][3] ? 0 : 2) : ((double)edgeLengthSq[1][3] < (double)edgeLengthSq[2][3] ? 1 : 2);
		det[15][3] = (float)((double)det[7][0] * (double)Dot(edges[index5][3], y[0]) + (double)det[7][1] * (double)Dot(edges[index5][3], y[1]) + (double)det[7][2] * (double)Dot(edges[index5][3], y[2]));
	}
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 심플렉스 업데이트
///@param newIndex : 새로 업데이트할 인덱스
//////////////////////////////////////////////////////////////////////////
bool Gjk::UpdateSimplex(int newIndex)
{
	int yBits = simplexBits | 1 << newIndex;
	int xBits = 1 << newIndex;
	for (int index = simplexBits; index != 0; --index)
	{
		if ((index & yBits) == index && IsSatisfiesRule(index | xBits, yBits))
		{
			simplexBits = index | xBits;
			closestPoint = ComputeClosestPoint();

			return true;
		}
	}

	bool flag = false;
	if (IsSatisfiesRule(xBits, yBits))
	{
		simplexBits = xBits;
		closestPoint = y[newIndex];
		maxLengthSq = yLengthSq[newIndex];
		flag = true;
	}

	return flag;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 가까운 위치 계산
///@return 결과 위치벡터
//////////////////////////////////////////////////////////////////////////
D3DXVECTOR3 Gjk::ComputeClosestPoint()
{
	float num1 = 0.0f;
	int num2 = BitsToIndices[simplexBits];
	D3DXVECTOR3 zero(0.f, 0.f, 0.f);
	maxLengthSq = 0.0f;

	while (num2 != 0)
	{
		int index = (num2 & 7) - 1;
		float num3 = det[simplexBits][index];
		num1 += num3;
		zero += y[index] * num3;
		maxLengthSq = Math::Max(maxLengthSq, yLengthSq[index]);
		num2 >>= 3;
	}
	return zero / num1;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief Bits값이 규칙에 맞는지 검사
///@param xBits : XBits
///@param yBits : YBits
///@return 결과
//////////////////////////////////////////////////////////////////////////
bool Gjk::IsSatisfiesRule(int xBits, int yBits)
{
	bool flag = true;
	int num1 = BitsToIndices[yBits];

	while (num1 != 0)
	{
		int index = (num1 & 7) - 1;
		int num2 = 1 << index;

		if ((num2 & xBits) != 0)
		{
			if ((double)det[xBits][index] <= 0.0)
			{
				flag = false;
				break;
			}
		}
		else if ((double)det[xBits | num2][index] > 0.0)
		{
			flag = false;
			break;
		}

		num1 >>= 3;
	}

	return flag;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 두 개의 벡터 내적
///@param a : 벡터1
///@param b : 벡터2
///@return 결과 값
//////////////////////////////////////////////////////////////////////////
float Gjk::Dot(D3DXVECTOR3 a, D3DXVECTOR3 b)
{
	return (float)((double)a.x * (double)b.x + (double)a.y * (double)b.y + (double)a.z * (double)b.z);
}

float Gjk::LengthSquared(D3DXVECTOR3 v)
{
	return (float)((double)v.x * (double)v.x+ (double)v.y * (double)v.y + (double)v.z * (double)v.z);
}
