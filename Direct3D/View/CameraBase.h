#pragma once
#include "./Object/GameObject/GameObject.h"
class BoundingFrustum;
class Perspective;

class CameraBase : public GameObject
{
protected:
	Synthesize(BoundingFrustum*, frustum,Frustum)
	Synthesize(Perspective*, perspective, Perspective)
	Synthesize(D3DXMATRIX, matView, ViewMatrix)
public:
	CameraBase();
	virtual ~CameraBase();

	virtual void Init();
	virtual void Release();
	virtual void Update();
	virtual void Render();

	virtual D3DXMATRIX GetProjection();
	class Ray GetRay();
};

