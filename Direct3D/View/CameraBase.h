#pragma once
class CameraBase
{
protected:
	Synthesize(class Transform*, transform, Transform)
	class Frustum* frustum;
	Synthesize(class Perspective*, perspective, Perspective)
	Synthesize(D3DXMATRIX, matView, ViewMatrix)

public:
	CameraBase();
	~CameraBase();

	virtual void Init();
	virtual void Relase();
	virtual void Update();
	virtual void Render();

	virtual D3DXMATRIX GetProjection();
};

