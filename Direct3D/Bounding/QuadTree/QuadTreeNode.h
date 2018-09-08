#pragma once

class QuadTreeNode
{
public:
	static UINT _renderingNodeCount;
private:
	enum CornerType { CORNER_LT = 0, CORNER_RT, CORNER_LB, CORNER_RB ,End};
	enum FrustumLocation {
		FRUSTUM_OUT = 0,
		FRUSTUM_PARTIALY_IN,
		FRUSTUM_COMPLETELY_IN,
		FRSUTUM_UNKOWN = -1
	};
private:
	int level;
	bool isInFrustum;
	class BoundingBox* boundingBox;
	vector<QuadTreeNode*> childs;
public:
	//TODO ���� ���� ���̰� ����� ����
	QuadTreeNode(int level , D3DXVECTOR3 minPos, D3DXVECTOR3 maxPos);			//���� ��� ������
	QuadTreeNode(QuadTreeNode* parent,CornerType cornerType);					//�ڽ� ��� ������
	~QuadTreeNode();

	void UpdateNode(class BoundingFrustum* pFrustum);
	void Render();
private:
	bool SubDevide();															//���� level�� 0���� ũ�ٸ� �и��Ѵ�.
	bool IsInFrustum(class BoundingFrustum* pFrustum);							//�������ҿ� ���ԵǴ°�.
	void SetIsInfrustum(bool b);
};

#define CanDeepInTo(level) level > 0