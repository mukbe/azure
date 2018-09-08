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
	class BoundingBox* boundingBox;
	vector<QuadTreeNode*> childs;
	vector<class StaticObject*> staticObjectList;
public:
	//TODO 추후 지형 높이가 들어갈경우 수정
	QuadTreeNode(int level , D3DXVECTOR3 minPos, D3DXVECTOR3 maxPos);			//최초 노드 생성자
	QuadTreeNode(QuadTreeNode* parent,CornerType cornerType);					//자식 노드 생성자
	~QuadTreeNode();

	void Render();
private:
	bool SubDevide();															//아직 level이 0보다 크다면 분리한다.
	bool IsInFrustum(class BoundingFrustum* pFrustum);							//프러스텀에 포함되는가.
};

#define CanDeepInTo(level) level > 0