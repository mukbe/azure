#pragma once
class Gjk
{
public:
	Gjk(void);

	bool FullSimplex() const;
	float MaxLengthSquared() const;
	D3DXVECTOR3 ClosestPoint() const;

	void Reset();

	bool AddSupportPoint(D3DXVECTOR3 newPoint);

	static float Dot(D3DXVECTOR3 a, D3DXVECTOR3 b);
	static float LengthSquared(D3DXVECTOR3 v);
private:
	void UpdateDeterminant(int index);
	bool UpdateSimplex(int newIndex);
	D3DXVECTOR3 ComputeClosestPoint();
	bool IsSatisfiesRule(int xBits, int yBits);
private:
	const static int BitsToIndices[16];

	D3DXVECTOR3 closestPoint; ///< 가까운 위치 벡터

	D3DXVECTOR3 y[4];///< y축 벡터
	float yLengthSq[4];///< y축 방향 크기

	D3DXVECTOR3 edges[4][4];///< 모서리 벡터
	float edgeLengthSq[4][4];///< 모서리 크기

	float det[16][4];///< Determinant

	int simplexBits;///< 심플렉스 비트(max:15)
	float maxLengthSq;///< 제곱으로 계산한 크기 중 가장 큰 값
};