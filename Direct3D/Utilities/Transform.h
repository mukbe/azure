#pragma once

#define RESET_POSITION 1
#define RESET_ROTATION 2
#define RESET_SCALE	   4

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

#define SHAKE_X 1
#define SHAKE_Y 2
#define SHAKE_Z 4

class Transform
{
	friend class DebugTransform;
private:
	union
	{
		struct { D3DXVECTOR3 axis[3]; };
		struct
		{
			D3DXVECTOR3 right;
			D3DXVECTOR3 up;
			D3DXVECTOR3 forward;
		};
	};

	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;

	D3DXMATRIX matFinal;
	D3DXMATRIX matLocal;

	Transform* pParent;
	Transform* pFirstChild;
	Transform* pNextSibling;

	D3DXVECTOR3 angle;

	bool bAutoUpdate;
public:
	Transform();
	~Transform();

	void RenderGizmo();

	D3DXMATRIX GetFinalMatrix()const { return this->matFinal; }
	D3DXMATRIX GetRotateMatrix();
	D3DXMATRIX GetScaleMatrix();
	D3DXMATRIX GetTranslationMatrix();
	void SetTransform(D3DXMATRIX mat);

	void UpdateTransform();
	void SetAutonUpdate(bool bAuto) { this->bAutoUpdate = bAuto; }

	void Reset(int resetFlag = -1);
	void AddChild(Transform* pNewChild);
	void AttachTo(Transform* pNewParent);
	void ReleaseParent();

	void SetWorldPosition(float x, float y, float z);
	void SetWorldPosition(D3DXVECTOR3 pos);

	void SetLocalPosition(float x, float y, float z);
	void SetLocalPosition(D3DXVECTOR3 pos);

	void MovePositionLocal(D3DXVECTOR3 delta);
	void MovePositionWorld(D3DXVECTOR3 delta);

	void SetScale(float x, float y, float z);
	void SetScale(D3DXVECTOR3 scale);

	void Scaling(float x, float y, float z);
	void Scaling(D3DXVECTOR3 deltaScale);

	void RotateSelf(float x, float y, float z);
	void RotateSelf(D3DXVECTOR3 angle);
	void Rotating(float x, float y, float z);
	void RotateWorld(float x, float y, float z);

	void LookDirection(D3DXVECTOR3 targetPos, D3DXVECTOR3 up);

	void PositionLerp(D3DXVECTOR3 from, D3DXVECTOR3 to, float t);

	D3DXVECTOR3 GetScale()const { return this->scale; }
	D3DXVECTOR3 GetWorldPosition()const;
	D3DXVECTOR3 GetLocalPosition()const;

	void GetUnitAxis(D3DXVECTOR3* pOut);
	D3DXVECTOR3 GetScaleAxis(int axisIndex)const;
	D3DXVECTOR3 GetUnitAxis(int axisIndex)const;
	D3DXVECTOR3 GetForward(bool bNormalize = true)const;
	D3DXVECTOR3 GetUp(bool bNormalize = true)const;
	D3DXVECTOR3 GetRight(bool bNormalize = true)const;
	D3DXVECTOR3 GetAngle()const { return this->angle; }

	void GuiRender(string name = "Transform");
};













