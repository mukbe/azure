#pragma once

#define ScaleOffset 10.0f

class DebugTransform
{
private:
	enum class PickType { None = 0,Right,Up,Forward,End };
	enum class DebugType { Translation = 0 ,Rotate,Scale,End};
	enum class SpaceType {World = 0,Local,End};
private:
	class CameraBase*		camera;
	class Transform*		transform;
	class BoundingBox*		axisBounding[3];
	
	PickType				pickType;
	DebugType				debugType;
	SpaceType				spaceType;

	D3DXVECTOR3				saveMousePos;
	D3DXVECTOR3				angle;
	D3DXVECTOR3				saveAngle;
public:
	DebugTransform();
	~DebugTransform();

	void ConnectTransform(class Transform* transform);
	void SetCamera(class CameraBase* camera) { this->camera = camera; }

	void Update();
	void RenderGizmo();
	void RenderGUI();

	class Transform* GetTransform()const { return this->transform; }
private:
	PickType IsPick();
	void ControlGizmo();
	void RenderAxisBounding();
};

