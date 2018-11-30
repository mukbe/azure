#include "stdafx.h"
#include "Math.h"

const float Math::PI = 3.14159265f;
const float Math::Epsilon = 1E-6f;
const int Math::IntMin = -2147483647;
const int Math::IntMax = 2147483647;
const float Math::FloatMin = -3.402823E+38f;
const float Math::FloatMax = 3.402823E+38f;

float Math::NegativeChecking(float value)
{
	if (value < 0.0f)
		return -1.0f;
	else
		return 1.0f;
}

float Math::ToRadian(float degree)
{
	return degree * PI / 180.0f;
}

float Math::ToDegree(float radian)
{
	return radian * 180.0f / PI;
}

float Math::Random(float r1, float r2)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = r2 - r1;
	float val = random * diff;
	
	return r1 + val;
}

float Math::RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}

float Math::Lerp(float val1, float val2, float amount)
{
	return val1 + (val2 - val1)* amount;
}

float Math::Clamp(float value, float min, float max)
{
	value = value > max ? max : value;
	value = value < min ? min : value;

	return value;
}


float Math::Modulo(float val1, float val2)
{
	while (val1 - val2 >= 0)
		val1 -= val2;

	return val1;
}

D3DXCOLOR Math::RandomColor()
{
	float r = Math::Random(0.0f, 1.0f);
	float g = Math::Random(0.f, 1.0f);
	float b = Math::Random(0.f, 1.0f);
	float a = Math::Random(0.3f, 0.5f);
	return D3DXCOLOR(r,g,b,a);
}


int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}


float Math::Angle(D3DXVECTOR3 v1, D3DXVECTOR3 v2)
{
	float angle = 0;

	float dot = D3DXVec3Dot(&v1, &v2);
	float length1 = D3DXVec3Length(&v1);
	float length2 = D3DXVec3Length(&v2);

	angle = acosf(dot / (length1 * length2));

	return angle;
}

void Math::ComputeNormal(vector<VertexTextureNormal>& vertexDatas,vector<UINT>& indexDatas)
{
	//초기화 하고 시작
	for (UINT i = 0; i < vertexDatas.size(); ++i)
		vertexDatas[i].normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//삼각형 갯수
	DWORD triNum = indexDatas.size() / 3;

	for (DWORD i = 0; i < triNum; i++)
	{
		//해당 삼각형의 정점 인덱스
		DWORD i0 = indexDatas[(i * 3) + 0];
		DWORD i1 = indexDatas[(i * 3) + 1];
		DWORD i2 = indexDatas[(i * 3) + 2];

		//정점 3개
		D3DXVECTOR3 v0 = vertexDatas[i0].position;
		D3DXVECTOR3 v1 = vertexDatas[i1].position;
		D3DXVECTOR3 v2 = vertexDatas[i2].position;

		//Edge
		D3DXVECTOR3 edge1 = v1 - v0;
		D3DXVECTOR3 edge2 = v2 - v0;

		//Cross
		D3DXVECTOR3 cross;
		D3DXVec3Cross(&cross, &edge1, &edge2);

		//Normal
		D3DXVECTOR3 normal;
		D3DXVec3Normalize(&normal, &cross);

		//증가 시킨다.
		vertexDatas[i0].normal += normal;
		vertexDatas[i1].normal += normal;
		vertexDatas[i2].normal += normal;
	}

	//최종 적으로 Normalvector 정규화한다.
	for (DWORD i = 0; i < vertexDatas.size(); i++)
		D3DXVec3Normalize(&vertexDatas[i].normal, &vertexDatas[i].normal);
}

void Math::ComputeTangentAngBinormal(vector<VertexTextureBlendNT>& vertexDatas, vector<UINT> inputIndicis, DWORD NumTris, DWORD NumVertices)
{
	
	//임시 Tangent Binormal 배열
	vector<D3DXVECTOR3> tangents, binormals;
	tangents.assign(NumVertices, D3DXVECTOR3(0.f,0.f,0.f));
	binormals.assign(NumVertices, D3DXVECTOR3(0.f,0.f,0.f));

	//일단 삼각형 수대로....
	for (DWORD a = 0; a < NumTris; a++)
	{
		//해당 삼각형의 인덱스
		DWORD i0 = inputIndicis[a * 3 + 0];
		DWORD i1 = inputIndicis[a * 3 + 1];
		DWORD i2 = inputIndicis[a * 3 + 2];

		//해당 삼각형의 정점위치
		D3DXVECTOR3 p0 = vertexDatas[i0].position;
		D3DXVECTOR3 p1 = vertexDatas[i1].position;
		D3DXVECTOR3 p2 = vertexDatas[i2].position;

		//해당 삼각형의 UV
		D3DXVECTOR2 uv0 = vertexDatas[i0].uv;
		D3DXVECTOR2 uv1 = vertexDatas[i1].uv;
		D3DXVECTOR2 uv2 = vertexDatas[i2].uv;

		//각변의 Edge
		D3DXVECTOR3 edge1 = p1 - p0;
		D3DXVECTOR3 edge2 = p2 - p0;

		//UV Edge
		D3DXVECTOR2 uvEdge1 = uv1 - uv0;
		D3DXVECTOR2 uvEdge2 = uv2 - uv0;

		// 위의 정보로 다음과 같은 공식이 성립
		// edge1 = ( uvEdge1.x ) * Tangent + ( uvEdge1.y ) * Binormal;
		// edge2 = ( uvEdge2.x ) * Tangent + ( uvEdge2.y ) * Binormal;

		// 다음과 같이 치환
		// E1 = edge1;
		// E2 = edge2;
		// U1 = uvEdge1.x;
		// V1 = uvEdge1.y;
		// U2 = uvEdge2.x;
		// V2 = uvEdge2.y;
		// T = Tangent;
		// B = Binormal;

		// E1 = U1 * T + V1 * B;
		// E2 = U2 * T + V2 * B;

		// | E1 |   | U1 , V1 | | T |
		// |    | = |         | |   |
		// | E2 |   | U2 , V2 | | B |

		// | T |          1        |  V2 , -V1 | | E1 |
		// |   | = --------------- |           | |    |
		// | B |    U1*V2 - V1*U2  | -U2 ,  U1 | | E2 |

		// R = 1 / U1*V2 - V1*U2;

		// T = ( ( E1 * V2 ) + ( E2 * -V1 ) ) * R
		// B = ( ( E1 * -U2 ) + ( E2 * U1 ) ) * R

		float r = 1.0f / ((uvEdge1.x * uvEdge2.y) - (uvEdge1.y * uvEdge2.x));

		//Tangent
		D3DXVECTOR3 t = ((edge1 * uvEdge2.y) + (edge2 * -uvEdge1.y)) * r;
		D3DXVec3Normalize(&t, &t);

		//Binormal 
		D3DXVECTOR3 b = ((edge1 * -uvEdge2.x) + (edge2 * uvEdge1.x)) * r;
		D3DXVec3Normalize(&b, &b);

		//탄젠트 바이노말 임시 배열에 추가
		tangents[i0] += t;
		tangents[i1] += t;
		tangents[i2] += t;
		binormals[i0] += b;
		binormals[i1] += b;
		binormals[i2] += b;
	}



	//Binormal Tangent 노말에 직교화
	for (int i = 0; i < NumVertices; i++) {

		D3DXVECTOR3 n = vertexDatas[i].normal;

		//Tangent 그람슈미트 직교
		D3DXVECTOR3 t = tangents[i] -
			(D3DXVec3Dot(&tangents[i], &n) * n);
		D3DXVec3Normalize(&t, &t);

		//노말과 직교화된 T 와 외적하여 B
		D3DXVECTOR3 b;
		D3DXVec3Cross(&b, &n, &t);
		D3DXVec3Normalize(&b, &b);

		vertexDatas[i].tangent = t;
	}

	tangents.clear();
	binormals.clear();
}

bool Math::IsPointInAABB(D3DXVECTOR2 rectPos, D3DXVECTOR2 rectSize, D3DXVECTOR2 ptPos)
{
	float left, top, right, bottom;
	left = rectPos.x;
	top = rectPos.y;
	right = rectPos.x + rectSize.x;
	bottom = rectPos.y - rectSize.y;

	if (ptPos.x < left || ptPos.x > right)
		return false;
	else if (ptPos.y < bottom || ptPos.y > top)
		return false;

	return true;
}
