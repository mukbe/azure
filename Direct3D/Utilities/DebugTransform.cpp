#include "stdafx.h"
#include "DebugTransform.h"

#include "./Utilities/Transform.h"
#include "./Bounding/BoundingBox.h"
#include "./Bounding/BoundingSphere.h"
#include "./Bounding/Ray.h"
#include "./View/CameraBase.h"
#include "./Utilities/Math.h"
DebugTransform::DebugTransform()
	:transform(NULL), debugType(DebugType::Translation),spaceType(SpaceType::Local),
	pickType(PickType::None),camera(NULL), saveMousePos(0.f, 0.f, 0.f),angle(0,0,0),saveAngle(0,0,0)
{
	axisBounding[AXIS_X] = new BoundingBox(D3DXVECTOR3(0.f, -0.5f, -0.5f), D3DXVECTOR3(1.f, 0.5f, 0.5f));
	axisBounding[AXIS_Y] = new BoundingBox(D3DXVECTOR3(-0.5f, 0.0f, -0.5f), D3DXVECTOR3(0.5f, 1.f, 0.5f));
	axisBounding[AXIS_Z] = new BoundingBox(D3DXVECTOR3(-0.5f, -0.5f, 0.f), D3DXVECTOR3(0.5f, 0.5f, 1.f));
}


DebugTransform::~DebugTransform()
{
	for (int i = 0; i < 3; ++i)
		SafeDelete(axisBounding[i]);
}

void DebugTransform::ConnectTransform(Transform * transform)
{
	this->transform = transform;
}

void DebugTransform::Update()
{
	if (this->transform != NULL)
	{
		if (KeyCode->Down(VK_LBUTTON))
			this->pickType = this->IsPick();
		if (KeyCode->Press(VK_LBUTTON))
			this->ControlGizmo();
		if (KeyCode->Up(VK_LBUTTON))
			this->pickType = PickType::None;

		this->saveMousePos = GameMouse->GetPosition();
	}
}

void DebugTransform::RenderGizmo()
{
	if (this->transform != NULL)
	{
		pRenderer->ChangeZBuffer(false);
		{
			D3DXCOLOR rightColor = ColorRed;
			D3DXCOLOR upColor = ColorGreen;
			D3DXCOLOR forwardColor = ColorBlue;

			if (pickType == PickType::Right)
				rightColor = ColorWhite;
			else if (pickType == PickType::Up)
				upColor = ColorWhite;
			else if (pickType == PickType::Forward)
				forwardColor = ColorWhite;

			if(this->spaceType == SpaceType::Local)
				GizmoRenderer->LocalGizmo(transform->position, 1.0f,
					transform->GetScaleAxis(AXIS_X) / 2.0f, transform->GetScaleAxis(AXIS_Y) / 2.0f, transform->GetScaleAxis(AXIS_Z) / 2.0f,
					rightColor, upColor, forwardColor);
			else
			{
				GizmoRenderer->LocalGizmo(transform->position, 1.0f,
					D3DXVECTOR3(0.5f,0.f,0.f) * transform->scale.x, 
					D3DXVECTOR3(0.f,0.5f,0.f) * transform->scale.y,
					D3DXVECTOR3(0.f,0.f,0.5f) * transform->scale.z,
					rightColor, upColor, forwardColor);
			}

			//this->DrawAxisBounding();
		}

		pRenderer->ChangeZBuffer(true);
	}
}

void DebugTransform::RenderGUI()
{
	if (this->transform != NULL)
	{
		ImGui::Begin("Transform");
		{
			ImGui::RadioButton("World", reinterpret_cast<int*>(&spaceType), 0); ImGui::SameLine();
			ImGui::RadioButton("Local", reinterpret_cast<int*>(&spaceType), 1);
			ImGui::Separator();

			ImGui::InputFloat3("Position", &transform->position.x);
			ImGui::InputFloat3("Scale", &transform->scale.x);
			ImGui::InputFloat3("Rotate", &angle.x);
			ImGui::Separator();

			ImGui::SliderFloat3("Pos", &transform->position.x,-100.f,100.f);
			ImGui::SliderFloat3("Scaled", &transform->scale.x,0.01f,10.f);
			ImGui::SliderFloat3("Rot", &angle.x,-6.28f,6.28f);
		}
		ImGui::End();

		if (this->angle != this->saveAngle)
		{
			transform->RotateSelf(this->saveAngle - angle);
		}

		this->transform->UpdateTransform();
		this->saveAngle = this->angle;
	}
}

DebugTransform::PickType DebugTransform::IsPick()
{
	//   0-------1
	//  /|      /|
	// 4-------5 |
	// | 3-----|-2
	// |/      |/
	// 7-------6 

	D3DXMATRIX scale, translation, rotate;
	translation = transform->GetTranslationMatrix();

	if (spaceType == SpaceType::Local)
		rotate = transform->GetRotateMatrix();
	else
		D3DXMatrixIdentity(&rotate);

	Ray ray = camera->GetRay();

	for (int i = 0; i < 3; ++i)
	{
		bool isPick = false;

		if (i == 0)
			D3DXMatrixScaling(&scale, transform->scale.x * 0.5f, 1.f, 1.f);
		else if (i == 1)
			D3DXMatrixScaling(&scale, 1.f, transform->scale.y * 0.5f, 1.f);
		else if (i == 2)
			D3DXMatrixScaling(&scale, 1.f, 1.f, transform->scale.z * 0.5f);
		D3DXMATRIX finalMatrix = scale * rotate * translation;

		vector<D3DXVECTOR3> corners;
		axisBounding[i]->GetCorners(corners, finalMatrix);

		//충돌 검사 
		float padding;
		//앞면
		if (D3DXIntersectTri(&corners[4], &corners[5], &corners[7], &ray.origin, &ray.direction, &padding, &padding, &padding)
			|| D3DXIntersectTri(&corners[7], &corners[5], &corners[6], &ray.origin, &ray.direction, &padding, &padding, &padding))
			isPick = true;
		//뒷면
		if (D3DXIntersectTri(&corners[0], &corners[1], &corners[3], &ray.origin, &ray.direction, &padding, &padding, &padding)
			|| D3DXIntersectTri(&corners[3], &corners[1], &corners[2], &ray.origin, &ray.direction, &padding, &padding, &padding))
			isPick = true;
		//왼쪽
		if (D3DXIntersectTri(&corners[7], &corners[4], &corners[0], &ray.origin, &ray.direction, &padding, &padding, &padding)
			|| D3DXIntersectTri(&corners[7], &corners[0], &corners[3], &ray.origin, &ray.direction, &padding, &padding, &padding))
			isPick = true;
		//우측
		if (D3DXIntersectTri(&corners[6], &corners[5], &corners[1], &ray.origin, &ray.direction, &padding, &padding, &padding)
			|| D3DXIntersectTri(&corners[6], &corners[1], &corners[2], &ray.origin, &ray.direction, &padding, &padding, &padding))
			isPick = true;
		//윗면
		if (D3DXIntersectTri(&corners[0], &corners[1], &corners[4], &ray.origin, &ray.direction, &padding, &padding, &padding)
			|| D3DXIntersectTri(&corners[4], &corners[1], &corners[5], &ray.origin, &ray.direction, &padding, &padding, &padding))
			isPick = true;
		//아래면
		if (D3DXIntersectTri(&corners[3], &corners[2], &corners[7], &ray.origin, &ray.direction, &padding, &padding, &padding)
			|| D3DXIntersectTri(&corners[7], &corners[2], &corners[6], &ray.origin, &ray.direction, &padding, &padding, &padding))
			isPick = true;

		corners.clear();

		if (isPick == true)
		{
			if (i == AXIS_X)return PickType::Right;
			else if (i == AXIS_Y)return PickType::Up;
			else if (i == AXIS_Z)return PickType::Forward;
		}
	}

	return PickType::None;
}

void DebugTransform::ControlGizmo()
{
	D3DXVECTOR3 moveLocation = GameMouse->GetPosition() - this->saveMousePos;
	moveLocation.y = -moveLocation.y;
	//Translation
	if (debugType == DebugType::Translation)
	{
		if (spaceType == SpaceType::Local)
		{
			float factor;
			if (this->pickType == PickType::Right)
			{
				factor = Math::NegativeChecking(D3DXVec3Dot(&(camera->GetTransform()->GetRight()), &transform->GetRight()));
				transform->MovePositionLocal(D3DXVECTOR3(factor * (moveLocation.x + moveLocation.y) * 0.1f, 0.f, 0.f));
			}
			else if (this->pickType == PickType::Up)
			{
				factor = Math::NegativeChecking(D3DXVec3Dot(&(camera->GetTransform()->GetUp()), &transform->GetUp()));
				transform->MovePositionLocal(D3DXVECTOR3(0.f, factor * (moveLocation.x + moveLocation.y)* 0.1f, 0.f));
			}
			else if (this->pickType == PickType::Forward)
			{
				factor = Math::NegativeChecking(D3DXVec3Dot(&(camera->GetTransform()->GetForward()), &transform->GetForward()));
				transform->MovePositionLocal(D3DXVECTOR3(0.f, 0.f, factor * (moveLocation.x + moveLocation.y) * 0.1f));
			}
		}
		else
		{
			if (this->pickType == PickType::Right)
			{
				transform->MovePositionWorld(D3DXVECTOR3( (moveLocation.x + moveLocation.y) * 0.1f, 0.f, 0.f));
			}
			else if (this->pickType == PickType::Up)
			{
				transform->MovePositionWorld(D3DXVECTOR3(0.f,  (moveLocation.x + moveLocation.y)* 0.1f, 0.f));
			}
			else if (this->pickType == PickType::Forward)
			{
				transform->MovePositionWorld(D3DXVECTOR3(0.f, 0.f,(moveLocation.x + moveLocation.y) * 0.1f));
			}
		}
	}
}

void DebugTransform::RenderAxisBounding()
{
	//Debug용도Axis피킹
	D3DXMATRIX scale, translation, rotate;
	translation = transform->GetTranslationMatrix();
	if (spaceType == SpaceType::Local)
		rotate = transform->GetRotateMatrix();
	else
		D3DXMatrixIdentity(&rotate);

	for (int i = 0; i < 3; ++i)
	{
		if (i == 0)
			D3DXMatrixScaling(&scale, transform->scale.x * 0.5f, 1.f, 1.f);
		else if (i == 1)
			D3DXMatrixScaling(&scale, 1.f, transform->scale.y * 0.5f, 1.f);
		else if (i == 2)
			D3DXMatrixScaling(&scale, 1.f, 1.f, transform->scale.z * 0.5f);
		D3DXMATRIX finalMatrix = scale * rotate * translation;
		axisBounding[i]->Render(finalMatrix, false, ColorWhite);
	}
}
