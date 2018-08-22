#pragma once
#include "RigidShapes.h"
#include <unordered_map>


//program에서 생성, 업데이트, 랜더링

class DebugDraw;
class Physics
{
private:
	class DebugAction : public btActionInterface
	{
	public:
		virtual void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep) {}

		virtual void debugDraw(btIDebugDraw* debugDrawer);
	};

public:
	static Physics* Get();
	static void Create();
	static void Delete();

	Physics();
	~Physics();
	
	void Update();
	void ImguiRender();
	void Render();

	void SimpleGizmo();

	void SetCamera(class CameraBase* cam) { camera = cam; }

	//activation 기본값 1
	void AddObject(IShapes& body, size_t* outID = nullptr, int activation = 1);
	//bTemp는 이 관리클래스에 있는 container에서도 삭제를 할것이냐 아니냐를 결정
	void RemoveObject(size_t id, bool bTemp = false);
	void Raycast();
	void GetRay(D3DXVECTOR3 * origin, D3DXVECTOR3* dir);

	//많은 씬을 생성해서 사용할 경우 world선언을 바꿔서 사용하길 바람 
	//주의할껀 world를 꼭 하나씩만 돌릴것 
	btDiscreteDynamicsWorld* GetWorld() { return world; }

	// DynamicsWorld에서 직접 ObjectArray를 가져다가 쓰는게 빠르긴 하다 
	btCollisionObject* GetCollisionObject(size_t id);

	//주로 쓰일함수 
	D3DXQUATERNION GetQuaternion(size_t id);
	D3DXVECTOR3 GetRotation(size_t id);
	D3DXVECTOR3 GetPosition(size_t id);
	D3DXMATRIX GetMatrix(size_t id);
	float GetMass(size_t id);

	//속도부여(마찰은 기본적으로 0이 아니므로 계속 움직이게 하려면 friction재설정요구) 
	//알고있는 물리적 운동량이랑 차이가 있다. 질량과 관계없고 x를 5로 설정했다면 5까지 선형보간으로 움직이면서 마찰이 조금 적용된다
	void SetVelocity(size_t id, D3DXVECTOR3 value);
	//각속도부여 (이것도 마찰의 영향을 받는다)
	void SetAngleVelocity(size_t id, D3DXVECTOR3 value);
	//순간이동
	void SetPosition(size_t id, D3DXVECTOR3 value);
	//물체의 기본축 변경 그러니까 up, forward, right를 변경
	void SetQuaternion(size_t id, D3DXQUATERNION value);
	//0으로는 변경불가
	void SetMass(size_t id, float m);

	//TODO 드래그해서 Velocity부여
	//view를 이용하면 될것같다

public:

	size_t GetPickedID()
	{
		if (pick != nullptr)
			return reinterpret_cast<size_t>(pick);

		return 0;
	}

	//Picked Info
	const btCollisionObject* GetPickedCollisionObject() { return pick; }
	D3DXQUATERNION GetPickedQuaternion() { return GetQuaternion(GetPickedID()); }
	D3DXVECTOR3 GetPickedRotation() { return GetRotation(GetPickedID()); }
	D3DXVECTOR3 GetPickedPosition() { return GetPosition(GetPickedID()); }
	D3DXMATRIX GetPickedMatrix() { return GetMatrix(GetPickedID()); }
	float GetPickedMass() { return GetMass(GetPickedID()); }

	//TODO 충돌된객체 콜백

private:
	static Physics* instance;
	static const btCollisionObject* pick;

	D3DXVECTOR3 quat_2_euler_d3d(const D3DXQUATERNION& q)
	{
		float sqw = q.w*q.w;
		float sqx = q.x*q.x;
		float sqy = q.y*q.y;
		float sqz = q.z*q.z;
		D3DXVECTOR3 rot;
		rot.x = asinf(2.0f * (q.w*q.x - q.y*q.z)); // rotation about x-axis
		rot.y = atan2f(2.0f * (q.x*q.z + q.w*q.y), (-sqx - sqy + sqz + sqw)); // rotation about y-axis
		rot.z = atan2f(2.0f * (q.x*q.y + q.w*q.z), (-sqx + sqy - sqz + sqw)); // rotation about z-axis
		return rot;
	}

private:
	btIDebugDraw* debugdraw;

	class CameraBase* camera;

	btDiscreteDynamicsWorld * world;
	btVector3 gravity;
	bool bRunning;

	unordered_map<size_t, btCollisionObject*> contain;
};

