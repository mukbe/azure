#pragma once
class CameraBase
{
protected:
	Synthesize(class Transform*, transform, Transform)
	class Frustum* frustum;
	class Perspective* perspective;
	Synthesize(D3DXMATRIX, matView, ViewMatrix)
	Synthesize(D3DXMATRIX, matViewProj, ViewProjection)
	class ViewProjectionBuffer* viewProjectionBuffer;
public:
	CameraBase();
	~CameraBase();

	virtual void Init();
	virtual void Relase();
	virtual void Update();
	virtual void Render();
};

