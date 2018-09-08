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

	D3DXVECTOR3 closestPoint; ///< ����� ��ġ ����

	D3DXVECTOR3 y[4];///< y�� ����
	float yLengthSq[4];///< y�� ���� ũ��

	D3DXVECTOR3 edges[4][4];///< �𼭸� ����
	float edgeLengthSq[4][4];///< �𼭸� ũ��

	float det[16][4];///< Determinant

	int simplexBits;///< ���÷��� ��Ʈ(max:15)
	float maxLengthSq;///< �������� ����� ũ�� �� ���� ū ��
};