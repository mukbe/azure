#pragma once
#include "CameraBase.h"


class FreeCamera : public CameraBase
{
private:
	Synthesize(float, nowAngleH, NowAngleH)
	Synthesize(float, nowAngleV, NowAngleV)
	Synthesize(float, maxAngleV, MaxAngleV)
	Synthesize(float, minAngleV, MinAngleV)
	Synthesize(float, nowSpeed, NowSpeed)
	Synthesize(float, maxSpeed, MaxSpeed)
public:
	FreeCamera();
	~FreeCamera();

	virtual void Init();
	virtual void Releaes();
	virtual void Update();
	virtual void Render();
	virtual void UIRender();
private:
	void ControlCamera();
	void UpdateView();
};

