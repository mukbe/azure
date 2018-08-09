#include "stdafx.h"
#include "Transform.h"


Transform::Transform()
	:pParent(NULL), pFirstChild(NULL), pNextSibling(NULL), bAutoUpdate(true)
{
	this->Reset(1);
	this->Reset(2);
	this->Reset(4);
}


Transform::~Transform()
{

}

void Transform::SetTransform(D3DXMATRIX mat)
{
	D3DXVECTOR3 x, y, z;
	memcpy(&x, &mat._11, sizeof D3DXVECTOR3);
	memcpy(&y, &mat._21, sizeof D3DXVECTOR3);
	memcpy(&z, &mat._31, sizeof D3DXVECTOR3);

	this->scale.x = D3DXVec3Length(&x);
	this->scale.y = D3DXVec3Length(&y);
	this->scale.z = D3DXVec3Length(&z);

	this->right = x / scale.x;
	this->up = y / scale.y;
	this->forward = z / scale.z;

	memcpy(&this->position, &mat._41, sizeof D3DXVECTOR3);

	angle = D3DXVECTOR3(0.f, 0.f, 0.f);

	this->UpdateTransform();

}

void Transform::UpdateTransform()
{
	D3DXMatrixIdentity(&matLocal);

	D3DXVECTOR3 scaleRight = this->right * this->scale.x;
	D3DXVECTOR3 scaleUp = this->up * this->scale.y;
	D3DXVECTOR3 scaleForward = this->forward * this->scale.z;

	memcpy(&matLocal._11, &scaleRight, sizeof D3DXVECTOR3);
	memcpy(&matLocal._21, &scaleUp, sizeof D3DXVECTOR3);
	memcpy(&matLocal._31, &scaleForward, sizeof D3DXVECTOR3);
	memcpy(&matLocal._41, &position, sizeof D3DXVECTOR3);

	if (this->pParent == NULL)
		this->matFinal = this->matLocal;
	else
		this->matFinal = matLocal * this->pParent->matFinal;

	Transform* child = this->pFirstChild;

	while (child != NULL)
	{
		child->UpdateTransform();
		child = child->pNextSibling;
	}
}

void Transform::Reset(int resetFlag)
{
	if (resetFlag & RESET_POSITION)
	{
		this->position.x = 0.f;
		this->position.y = 0.f;
		this->position.z = 0.f;
	}
	if (resetFlag & RESET_ROTATION)
	{
		this->right = D3DXVECTOR3(1.f, 0.f, 0.f);
		this->up = D3DXVECTOR3(0.f, 1.f, 0.f);
		this->forward = D3DXVECTOR3(0.f, 0.f, 1.f);
		this->angle = D3DXVECTOR3(0.f, 0.f, 0.f);
	}
	if (resetFlag & RESET_SCALE)
	{
		this->scale.x = 1.f;
		this->scale.y = 1.f;
		this->scale.z = 1.f;
	}

	this->UpdateTransform();
}

void Transform::AddChild(Transform * pNewChild)
{
	if (pNewChild->pParent == this)
		return;

	pNewChild->ReleaseParent();

	//부모의 상대적인 좌표값으로 갱신하기 위해 부모의 역행렬을 구한다. 
	D3DXMATRIX matInvFinal;
	D3DXMatrixInverse(&matInvFinal, NULL, &this->matFinal);

	//자식의 속성들 갱신
	D3DXVec3TransformCoord(&pNewChild->position, &pNewChild->position, &matInvFinal);
	for (int i = 0; i < 3; ++i)
		D3DXVec3TransformNormal(pNewChild->axis + i, pNewChild->axis + i, &matInvFinal);
	pNewChild->scale.x = D3DXVec3Length(&pNewChild->right);
	pNewChild->scale.y = D3DXVec3Length(&pNewChild->up);
	pNewChild->scale.z = D3DXVec3Length(&pNewChild->forward);

	pNewChild->pParent = this;

	Transform* pChild = this->pFirstChild;

	if (pChild == NULL)
	{
		this->pFirstChild = pNewChild;
		pNewChild->pParent = this;
	}
	else
	{
		while (pChild != NULL)
		{
			if (pChild->pNextSibling == NULL)
			{
				pChild->pNextSibling = pNewChild;
				pNewChild->pParent = this;
				break;
			}
			pChild = pChild->pNextSibling;
		}
	}

	this->UpdateTransform();
}

void Transform::AttachTo(Transform * pNewParent)
{
	pNewParent->AddChild(this);
}

void Transform::ReleaseParent()
{
	if (this->pParent == NULL)
		return;

	Transform* pChild = this->pParent->pFirstChild;

	if (pChild == this)		//내가 부모의 첫번째 자식인가
	{
		this->pParent->pFirstChild = this->pNextSibling;

		this->pNextSibling = NULL;
	}
	else
	{

	}
	{
		while (pChild != NULL)
		{
			if (pChild->pNextSibling == this)
			{
				pChild->pNextSibling = this->pNextSibling;
				this->pNextSibling = NULL;
				break;
			}

			pChild = pChild->pNextSibling;
		}
	}

	this->pParent = NULL;

	//계층구조를 파기했으면 내 월드에 대한 갱신도 필요하다. 
	//matFinal 정보대로 갱신

	//월드 갱신 
	this->position.x = this->matFinal._41;
	this->position.y = this->matFinal._42;
	this->position.z = this->matFinal._43;

	D3DXVECTOR3 scaleForward(this->matFinal._31, this->matFinal._32, this->matFinal._33);
	D3DXVECTOR3 scaleUp(this->matFinal._21, this->matFinal._22, this->matFinal._23);
	D3DXVECTOR3 scaleRight(this->matFinal._11, this->matFinal._12, this->matFinal._13);

	this->scale.x = D3DXVec3Length(&scaleRight);
	this->scale.y = D3DXVec3Length(&scaleUp);
	this->scale.z = D3DXVec3Length(&scaleForward);

	D3DXVec3Normalize(&this->right, &scaleRight);
	D3DXVec3Normalize(&this->up, &scaleUp);
	D3DXVec3Normalize(&this->forward, &scaleForward);
}

void Transform::SetWorldPosition(float x, float y, float z)
{
	D3DXVECTOR3 worldPos(x, y, z);

	if (this->pParent != NULL)
	{
		D3DXMATRIX matParentInvFinal;
		D3DXMatrixInverse(&matParentInvFinal, NULL, &pParent->matFinal);

		D3DXVec3TransformCoord(&this->position, &worldPos, &matParentInvFinal);
	}
	else
	{
		this->position = worldPos;
	}

	if (this->bAutoUpdate)
		this->UpdateTransform();
}

void Transform::SetWorldPosition(D3DXVECTOR3 pos)
{
	this->SetWorldPosition(pos.x, pos.y, pos.z);
}

void Transform::SetLocalPosition(float x, float y, float z)
{
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;

	if (this->bAutoUpdate)
		this->UpdateTransform();
}

void Transform::SetLocalPosition(D3DXVECTOR3 pos)
{
	this->SetLocalPosition(pos.x, pos.y, pos.z);
}

void Transform::MovePositionLocal(D3DXVECTOR3 delta)
{
	D3DXVECTOR3 move(0.f, 0.f, 0.f);
	D3DXVECTOR3 moveAxis[3];

	this->GetUnitAxis(moveAxis);

	move += moveAxis[0] * delta.x;
	move += moveAxis[1] * delta.y;
	move += moveAxis[2] * delta.z;

	D3DXVECTOR3 nowPos = this->GetWorldPosition();
	this->SetWorldPosition(nowPos + move);
}

void Transform::MovePositionWorld(D3DXVECTOR3 delta)
{
	this->SetWorldPosition(this->GetWorldPosition() + delta);
}

void Transform::SetScale(float x, float y, float z)
{
	this->scale.x = x;
	this->scale.y = y;
	this->scale.z = z;

	if (this->bAutoUpdate)
		this->UpdateTransform();
}

void Transform::SetScale(D3DXVECTOR3 scale)
{
	this->scale.x = scale.x;
	this->scale.y = scale.y;
	this->scale.z = scale.z;

	if (this->bAutoUpdate)
		this->UpdateTransform();
}

void Transform::Scaling(float x, float y, float z)
{
	this->scale.x += x;
	this->scale.y += y;
	this->scale.z += z;
	if (this->bAutoUpdate)
		this->UpdateTransform();
}

void Transform::Scaling(D3DXVECTOR3 deltaScale)
{
	this->scale += deltaScale;
	if (this->bAutoUpdate)
		this->UpdateTransform();
}

void Transform::RotateSelf(float x, float y, float z)
{
	this->angle.x += x;
	this->angle.y += y;
	this->angle.z += z;

	D3DXMATRIX matRotateX;
	D3DXMatrixRotationAxis(&matRotateX, &this->right, x);
	D3DXMATRIX matRotateY;
	D3DXMatrixRotationAxis(&matRotateY, &this->up, y);
	D3DXMATRIX matRotateZ;
	D3DXMatrixRotationAxis(&matRotateZ, &this->forward, z);

	D3DXMATRIX matRotate;
	D3DXMatrixMultiply(&matRotate, &matRotateY, &matRotateX);
	D3DXMatrixMultiply(&matRotate, &matRotate, &matRotateZ);

	for (int i = 0; i < 3; ++i)
		D3DXVec3TransformNormal(&this->axis[i], &this->axis[i], &matRotate);

	if (this->bAutoUpdate)
		this->UpdateTransform();
}

void Transform::Rotating(float x, float y, float z)
{
	this->angle += D3DXVECTOR3(x, y, z);

	//사원수 준비
	D3DXQUATERNION quatRot;
	D3DXQuaternionRotationYawPitchRoll(
		&quatRot, angle.x, angle.y, angle.z);

	//사원수에 의한 행렬 준비
	D3DXMATRIX matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quatRot);

	//부모가 있다면 부모의 상대적인 위치로...
	if (this->pParent != NULL)
	{
		D3DXMATRIX matInvParentFinal;
		D3DXMatrixInverse(&matInvParentFinal, NULL, &this->pParent->matFinal);

		//부모의 역행렬에 곱
		D3DXMatrixMultiply(&matRotate, &matRotate, &matInvParentFinal);
	}


	//축 리셋
	this->right = D3DXVECTOR3(1, 0, 0);
	this->up = D3DXVECTOR3(0, 1, 0);
	this->forward = D3DXVECTOR3(0, 0, 1);

	this->angle.x = x;
	this->angle.y = y;
	this->angle.z = z;

	//최종 회전 행렬대로 회전 시킨다.
	for (int i = 0; i < 3; i++)
		D3DXVec3TransformNormal(
			&this->axis[i],
			&this->axis[i],
			&matRotate);

	if (this->bAutoUpdate)
		this->UpdateTransform();
}

void Transform::RotateWorld(float x, float y, float z)
{
	//사원수 준비
	D3DXQUATERNION quatRot;
	D3DXQuaternionRotationYawPitchRoll(
		&quatRot, y, x, z);

	//사원수에 의한 행렬 준비
	D3DXMATRIX matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quatRot);

	//부모가 있다면 부모의 상대적인 위치로...
	if (this->pParent != NULL)
	{
		D3DXMATRIX matInvParentFinal;
		D3DXMatrixInverse(&matInvParentFinal, NULL, &this->pParent->matFinal);

		//부모의 역행렬에 곱
		D3DXMatrixMultiply(&matRotate, &matRotate, &matInvParentFinal);
	}


	//축 리셋
	this->right = D3DXVECTOR3(1, 0, 0);
	this->up = D3DXVECTOR3(0, 1, 0);
	this->forward = D3DXVECTOR3(0, 0, 1);

	this->angle.x = x;
	this->angle.y = y;
	this->angle.z = z;

	//최종 회전 행렬대로 회전 시킨다.
	for (int i = 0; i < 3; i++)
		D3DXVec3TransformNormal(
			&this->axis[i],
			&this->axis[i],
			&matRotate);

	if (this->bAutoUpdate)
		this->UpdateTransform();
}

void Transform::LookDirection(D3DXVECTOR3 targetPos, D3DXVECTOR3 up)
{
	//위치에 대한 방향 벡터를 얻는다;
	D3DXVECTOR3 worldPos = this->GetWorldPosition();
	D3DXVECTOR3 dir = targetPos - worldPos; //forward;
	D3DXVec3Normalize(&dir, &dir);

	//오른쪽 벡터 
	D3DXVECTOR3 newRight;
	D3DXVec3Cross(&newRight, &up, &dir);
	D3DXVec3Normalize(&newRight, &newRight);

	//업 벡터 다시 계산
	D3DXVECTOR3 newUp;
	D3DXVec3Cross(&newUp, &dir, &newRight);
	D3DXVec3Normalize(&newUp, &newUp);

	if (this->pParent)
	{
		//새로운 축 성분에 부모 역행렬 곱
		D3DXMATRIX matInvParentFinal;
		D3DXMatrixInverse(&matInvParentFinal, NULL,
			&this->pParent->matFinal);

		D3DXVec3TransformNormal(&this->forward,
			&dir, &matInvParentFinal);
		D3DXVec3TransformNormal(&this->up,
			&newUp, &matInvParentFinal);
		D3DXVec3TransformNormal(&this->right,
			&newRight, &matInvParentFinal);
	}
	else
	{
		this->forward = dir;
		this->right = newRight;
		this->up = newUp;
	}

	if (this->bAutoUpdate)
		this->UpdateTransform();
}

void Transform::PositionLerp(D3DXVECTOR3 from, D3DXVECTOR3 to, float t)
{
	t = Math::Clamp(t,0.f,1.f);

	if (FLOATZERO(t))
	{
		this->SetWorldPosition(from);
	}
	else if (FLOATQUAL(t, 1.0f))
	{
		this->SetWorldPosition(to);
	}
	else
	{
		D3DXVECTOR3 result;
		D3DXVec3Lerp(&result, &from, &to, t);
		this->SetWorldPosition(result);
	}
}

D3DXVECTOR3 Transform::GetWorldPosition() const
{
	D3DXVECTOR3 pos = this->position;
	if (this->pParent) {
		D3DXVec3TransformCoord(&pos, &pos,
			&this->pParent->matFinal);
	}
	return pos;
}

D3DXVECTOR3 Transform::GetLocalPosition() const
{
	return this->position;
}

void Transform::GetUnitAxis(D3DXVECTOR3 * pOut)
{
	for (int i = 0; i < 3; i++)
		D3DXVec3Normalize(pOut + i, this->axis + i);

	if (this->pParent)
	{
		D3DXMATRIX matParentFinal = this->pParent->matFinal;
		for (int i = 0; i < 3; i++)
		{
			D3DXVec3TransformNormal(&pOut[i], &pOut[i],
				&matParentFinal);
		}
	}
}

D3DXVECTOR3 Transform::GetScaleAxis(int axisIndex) const
{
	D3DXVECTOR3 result = this->axis[axisIndex];

	//부모가 있다면..
	if (this->pParent)
	{
		D3DXMATRIXA16 matParentFinal = this->pParent->matFinal;
		D3DXVec3TransformNormal(&result, &result, &matParentFinal);
	}

	return result;
}

D3DXVECTOR3 Transform::GetUnitAxis(int axisIndex) const
{
	//부모가 있다면..
	if (this->pParent)
	{
		D3DXVECTOR3 result;
		D3DXMATRIXA16 matParentFinal = this->pParent->matFinal;
		D3DXVec3TransformNormal(&result, &this->axis[axisIndex], &matParentFinal);
		return result;
	}
	else
	{
		return this->axis[axisIndex];
	}


}

D3DXVECTOR3 Transform::GetForward(bool bNormalize) const
{
	if (bNormalize)
		return this->GetUnitAxis(AXIS_Z);

	return this->GetScaleAxis(AXIS_Z);
}

D3DXVECTOR3 Transform::GetUp(bool bNormalize) const
{
	if (bNormalize)
		return this->GetUnitAxis(AXIS_Y);

	return this->GetScaleAxis(AXIS_Y);
}

D3DXVECTOR3 Transform::GetRight(bool bNormalize) const
{
	if (bNormalize)
		return this->GetUnitAxis(AXIS_X);

	return this->GetScaleAxis(AXIS_X);
}

void Transform::GuiRender(string name /* = "Transform"*/)
{
	if (ImGui::Begin(name.c_str()))
	{
		ImGui::InputFloat3("Position", &position.x);
		ImGui::InputFloat3("Scale", &scale.x);

		ImGui::End();
	}
}








