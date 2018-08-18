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

	D3DXVECTOR2 point;			//��ȯ�� vector3
	D3DXVECTOR3 direction;		//��ȯ�� direction

	//viewport�� ����ȯ : ����ȯ�� �Ϸ��� �ݵ�� ����ȭ�� �����־�� �Ѵ�.
	point.x = ((2.0f * mouse.x) / screenSize.x) - 1.0f;
	point.y = (((2.0f * mouse.y) / screenSize.y) - 1.0f) * -1.0f;
	//����ȭ ��

	//projection ����ȯ
	//����ȣ�� �� ��Ʈ�� �̿��� �������� ����ȯ�Ѵ�
	//world->view->projection
	//����� �� ���ϹǷ�, ������ ������ ������. 
	D3DXMATRIX proj;
	perspective->GetMatrix(&proj);

	point.x = point.x / proj._11;
	point.y = point.y / proj._22;

	D3DXMATRIX inverseView;			//viewInverse
	D3DXMatrixInverse(&inverseView, NULL, &matView);

	//��ġ�� ���� ���Ѵ�.
	direction.x = (point.x * inverseView._11) + (point.y * inverseView._21) + inverseView._31;
	direction.y = (point.x * inverseView._12) + (point.y * inverseView._22) + inverseView._32;
	direction.z = (point.x * inverseView._13) + (point.y * inverseView._23) + inverseView._33;

	D3DXVec3Normalize(&direction, &direction);

	Ray ray;
	ray.origin = this->transform->GetWorldPosition();
	ray.direction = direction;
	return ray;
}
