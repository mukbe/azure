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

//���� �۾��� �䱸�� ��쿡�� ���� �ʴ´�
//��ü���� ������ ���� �̸� �����͸� �޾Ƽ� ����ϰų� 
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

