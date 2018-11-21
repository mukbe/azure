#pragma once

class CameraManager
{
	SingletonHeader(CameraManager)

private:
	typedef map<string, CameraBase*> CameraContainer;
	typedef map<string, CameraBase*>::iterator Iter;

public:

	CameraBase* AddCamera(string key, CameraBase* cam);
	BOOL DeleteCamera(string key);
	CameraBase* FindCamera(string key);
	BOOL ClearCamera();

	BOOL BindGPU(string key);
	BOOL BindGPU(CameraBase* cam);


	CameraContainer CameraContain;
	
};

#define Cameras CameraManager::Get()
