#pragma once
#include "CameraBase.h"
class FirstPersonCamera : public CameraBase
{
private:
	Synthesize(float, nowAngleH, NowAngleH)
	Synthesize(float, nowAngleV, NowAngleV)
	Synthesize(float, maxAngleV, MaxAngleV)
	Synthesize(float, minAngleV, MinAngleV)
public:
	FirstPersonCamera();
	~FirstPersonCamera();

	virtual void Init();
	virtual void Release();
	virtual void Update();
	virtual void Render();
	
	void ControlCamera();
	void UpdateView();
};

