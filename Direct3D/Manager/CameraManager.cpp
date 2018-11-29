#include "stdafx.h"
#include "CameraManager.h"
#include "./View/CameraBase.h"

SingletonCpp(CameraManager)


CameraManager::CameraManager()
{
}


CameraManager::~CameraManager()
{
}

CameraBase * CameraManager::AddCamera(string key, CameraBase * cam)
{
	if (FindCamera(key) == nullptr)
	{
		CameraContain.insert(make_pair(key, cam));
		return cam;
	}
	else
		return nullptr;
}

BOOL CameraManager::DeleteCamera(string key)
{
	CameraBase* cam = FindCamera(key);
	
	if (cam == nullptr)
		return FALSE;

	SafeDelete(cam);
	return TRUE;

}

CameraBase * CameraManager::FindCamera(string key)
{
	Iter iter = CameraContain.find(key);

	if (iter != CameraContain.end())
	{
		return iter->second;
	}

	return nullptr;
}

//빠른 작업을 요구할 경우에는 쓰지 않는다
//객체들을 생성할 때에 미리 포인터를 받아서 사용하거나 
BOOL CameraManager::BindGPU(string key)
{
	CameraBase* cam = FindCamera(key);

	if (cam == nullptr)
	{
		return FALSE;
	}

	cam->Render();
	return TRUE;

}
BOOL CameraManager::BindGPU(CameraBase * cam)
{
	cam->Render();
	return TRUE;
}

BOOL CameraManager::ClearCamera()
{
	Iter iter = CameraContain.begin();

	for (; iter != CameraContain.end(); ++iter)
	{
		SafeDelete(iter->second);
	}

	CameraContain.clear();
	return TRUE;
}

