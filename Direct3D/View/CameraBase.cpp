#include "stdafx.h"
#include "CameraBase.h"

#include "../Utilities/Transform.h"
#include "../Renders/WorldBuffer.h"
#include "Perspective.h"
#include "Bounding/Ray.h"

CameraBase::CameraBase()
	:GameObject("Camera")
{
	D3DDesc desc;
	DxRenderer::GetDesc(&desc);

	this->perspective = new Perspective((float)desc.Width, (float)desc.Height);

	this->transform->SetWorldPosition(0, 0, -20);

	D3DXMatrixLookAtLH(&matView, &transform->GetWorldPosition(), &(transform->GetWorldPosition() + transform->GetForward()), &D3DXVECTOR3(0.f, 1.f, 0.f));

	perspective->SetView(matView);

}

CameraBase::~CameraBase()
{
	SafeDelete(perspective);
	SafeDelete(transform);
}


void CameraBase::Init()
{
}

void CameraBase::Release()
{
}

void CameraBase::Update()
{
	GameObject::Update();
}

void CameraBase::Render()
{
	perspective->Render();
}

D3DXMATRIX CameraBase::GetProjection()
{
	return perspective->GetMatrix();
}

Ray CameraBase::GetRay()
{
	D3DDesc desc;
	DxRenderer::GetDesc(&desc);

	D3DXVECTOR2 screenSize;
	screenSize.x = desc.Width;
	screenSize.y = desc.Height;

	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	D3DXVECTOR2 point;			//변환할 vector3
	D3DXVECTOR3 direction;		//변환할 direction

	//viewport의 역변환 : 역변환을 하려면 반드시 정규화를 시켜주어야 한다.
	point.x = ((2.0f * mouse.x) / screenSize.x) - 1.0f;
	point.y = (((2.0f * mouse.y) / screenSize.y) - 1.0f) * -1.0f;
	//정규화 끝

	//projection 역변환
	//정규호한 뷰 포트를 이용해 프로젝을 역변환한다
	//world->view->projection
	//계산할 때 곱하므로, 역으로 갈려면 나눈다. 
	D3DXMATRIX proj;
	perspective->GetMatrix(&proj);

	point.x = point.x / proj._11;
	point.y = point.y / proj._22;

	D3DXMATRIX inverseView;			//viewInverse
	D3DXMatrixInverse(&inverseView, NULL, &matView);

	//위치에 축을 곱한다.
	direction.x = (point.x * inverseView._11) + (point.y * inverseView._21) + inverseView._31;
	direction.y = (point.x * inverseView._12) + (point.y * inverseView._22) + inverseView._32;
	direction.z = (point.x * inverseView._13) + (point.y * inverseView._23) + inverseView._33;

	D3DXVec3Normalize(&direction, &direction);

	Ray ray;
	ray.origin = this->transform->GetWorldPosition();
	ray.direction = direction;
	return ray;
}
