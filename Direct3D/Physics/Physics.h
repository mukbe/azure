#pragma once
#include "RigidShapes.h"
#include <unordered_map>


//program���� ����, ������Ʈ, ������

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

	//activation �⺻�� 1
	void AddObject(IShapes& body, size_t* outID = nullptr, int activation = 1);
	//bTemp�� �� ����Ŭ������ �ִ� container������ ������ �Ұ��̳� �ƴϳĸ� ����
	void RemoveObject(size_t id, bool bTemp = false);
	void Raycast();
	void GetRay(D3DXVECTOR3 * origin, D3DXVECTOR3* dir);

	//���� ���� �����ؼ� ����� ��� world������ �ٲ㼭 ����ϱ� �ٶ� 
	//�����Ҳ� world�� �� �ϳ����� ������ 
	btDiscreteDynamicsWorld* GetWorld() { return world; }

	// DynamicsWorld���� ���� ObjectArray�� �����ٰ� ���°� ������ �ϴ� 
	btCollisionObject* GetCollisionObject(size_t id);

	//�ַ� �����Լ� 
	D3DXQUATERNION GetQuaternion(size_t id);
	D3DXVECTOR3 GetRotation(size_t id);
	D3DXVECTOR3 GetPosition(size_t id);
	D3DXMATRIX GetMatrix(size_t id);
	float GetMass(size_t id);

	//�ӵ��ο�(������ �⺻������ 0�� �ƴϹǷ� ��� �����̰� �Ϸ��� friction�缳���䱸) 
	//�˰��ִ� ������ ����̶� ���̰� �ִ�. ������ ������� x�� 5�� �����ߴٸ� 5���� ������������ �����̸鼭 ������ ���� ����ȴ�
	void SetVelocity(size_t id, D3DXVECTOR3 value);
	//���ӵ��ο� (�̰͵� ������ ������ �޴´�)
	void SetAngleVelocity(size_t id, D3DXVECTOR3 value);
	//�����̵�
	void SetPosition(size_t id, D3DXVECTOR3 value);
	//��ü�� �⺻�� ���� �׷��ϱ� up, forward, right�� ����
	void SetQuaternion(size_t id, D3DXQUATERNION value);
	//0���δ� ����Ұ�
	void SetMass(size_t id, float m);

	//TODO �巡���ؼ� Velocity�ο�
	//view�� �̿��ϸ� �ɰͰ���

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

	//TODO �浹�Ȱ�ü �ݹ�

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

