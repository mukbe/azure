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
	vector<class StaticObject*> objectList;
public:
	QuadTreeNode(int level, class GameMap* pMap);
	QuadTreeNode(QuadTreeNode* pParent, CornerType type, class GameMap* pMap);
	~QuadTreeNode();

	void UpdateHeightData(class GameMap* pMap);

	void Update(class BoundingFrustum* pFrustum);
	void Render();
	void AddObject(class StaticObject* object);
private:
	bool SubDevide(class GameMap* pMap);															//아직 level이 0보다 크다면 분리한다.
	bool IsInFrustum(class BoundingFrustum* pFrustum);							//프러스텀에 포함되는가.
	void SetIsInfrustum(bool b);
};

#define CanDeepInTo(level) level > 0