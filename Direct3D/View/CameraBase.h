#pragma once
#include "./Object/GameObject/GameObject.h"
class CameraBase : public GameObject
{
protected:
	class Frustum* frustum;
	Synthesize(class Perspective*, perspective, Perspective)
	Synthesize(D3DXMATRIX, matView, ViewMatrix)
public:
	CameraBase();
	~CameraBase();

	virtual void Init();
	virtual void Release();
	virtual void Update();
	virtual void Render();

	virtual D3DXMATRIX GetProjection();
	class Ray GetRay();
};

