#include "stdafx.h"
#include "Physics.h"
#include "DebugDraw.h"

//raycast : ���̺귯������ btTriangleRaycastCallback�ν��� �ȵǼ� ���� �߰���
//btTriangleRaycastCallback : btTriangleCallback�� ��ӹ޾� ������� Ŭ����
#include "./BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "./View/CameraBase.h"

void Physics::DebugAction::debugDraw(btIDebugDraw* debugDrawer)
{
	//�������� �������� �𸣰ڴµ� ����ŷ������� ���� �ٲ�� ����
	//�ӽ÷� �浹������ �ൿ�� �߰��ص�
	((DebugDraw*)debugDrawer)->flushLines();
}

#define FLOAT_EQ(x,v) (((v - FLT_EPSILON) < x) && (x <( v + FLT_EPSILON))) 

Physics* Physics::instance = nullptr;
const btCollisionObject* Physics::pick = nullptr;

Physics::Physics()
	: gravity(0.f, -9.8f, 0.f) , bRunning(true), camera(nullptr)
{
	RenderRequest->AddRender("Physics::Render", bind(&Physics::Render, this),RenderType::Render);
	RenderRequest->AddRender("Physics::UIRender", bind(&Physics::ImguiRender, this), RenderType::UIRender);

	//�������
	btDbvtBroadphase* overlappingPairCache = new btDbvtBroadphase();
	
	btDefaultCollisionConfiguration* collisionConfig = new btDefaultCollisionConfiguration();

	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfig);

	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfig);
	

	world->setGravity(gravity);

	// ������� btIDebugDraw�� ��ӹ޾� ���������Լ��� ���� �������ָ� ��밡�� 
	debugdraw = new DebugDraw;

	debugdraw->setDebugMode(btIDebugDraw::DBG_DrawAabb | btIDebugDraw::DBG_DrawWireframe);
	debugdraw->setDebugMode(btIDebugDraw::DBG_NoDebug);

	world->setDebugDrawer(debugdraw);
	world->addAction(new DebugAction);
}


Physics::~Physics()
{
}

void Physics::Update()
{
	Raycast();


	if (bRunning)
	{
		//masSubSteps == 1 ����, == 0  ����
		//fixedTimeStep�� ���̸� ���귮�� �þ���� ���հ��� ���� �پ��
		world->stepSimulation(1.f / 60.f, 0);
		world->performDiscreteCollisionDetection();
	}
}

void Physics::ImguiRender()
{
	ImGui::Begin("Physics");
	if (ImGui::SliderFloat3("Gravity", &gravity[0], -30.f, 30.f))
	{
		for (int i = 0; i < world->getCollisionObjectArray().size();i++)
		{
			//������ ����ȿ� �ִ� �浹ü�� Ȱ��ȭ  �⺻������ false�� �Ǿ��ִµ� �� �̻��ϴ�...
			//�ϴ� �̷����ϸ� �ٲ� �߷��� �����
			//�����ÿ� �ٲ㵵 ������ ��� �̻��� ������ �𸣴� �ϴ� ���⿡�� ��� ��
			world->getCollisionObjectArray()[i]->activate(true);
		}
		world->setGravity(gravity);
	}
	
	//TODO üũ�������� �ٲ�ߵ� 
	static bool temp = true;
	if (ImGui::Checkbox("DrawMode", &temp))
	{
		if (temp != true)
		{
			debugdraw->setDebugMode(btIDebugDraw::DBG_NoDebug);
		}
		else
		{
			debugdraw->setDebugMode(btIDebugDraw::DBG_DrawAabb | btIDebugDraw::DBG_DrawWireframe);
		}
	}

	ImGui::Checkbox("World Running", &bRunning);
	if (pick != nullptr)
	{
		btVector3 pos = pick->getWorldTransform().getOrigin();
		D3DXVECTOR3 value;
		memcpy(&value, &pos, sizeof(D3DXVECTOR3));

		ImGui::Text("posX : %.2f , posY : %.2f , posZ : %.2f", value.x, value.y, value.z);

		
		float mass = GetPickedMass();
		ImGui::Text("Mass : %.2f", mass);

	}

	ImGui::End();
}

void Physics::Render()
{
	SimpleGizmo();

	if (debugdraw != nullptr)
		world->debugDrawWorld();
		//((DebugDraw*)debugdraw)->flushLines();
}

void Physics::SimpleGizmo()
{
	//��ŷ�ȳ� up,right,forward Draw
	{
		D3DXVECTOR3 up, right, forward;
		btVector3 pos;
		btVector3 btUp, btRight, btForward;
		btScalar factor = 3.f;
		if (pick != nullptr)
		{

			D3DXMATRIX mat = GetMatrix(GetPickedID());
			D3DXVec3TransformNormal(&up, &D3DXVECTOR3(0, 1, 0), &mat);
			D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1, 0, 0), &mat);
			D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0, 0, 1), &mat);
			memcpy(&pos, &mat._41, sizeof(float) * 3);
			pos.setW(0.f);

			memcpy(&btUp, &up, sizeof(float) * 3);
			memcpy(&btRight, &right, sizeof(float) * 3);
			memcpy(&btForward, &forward, sizeof(float) * 3);
			btUp.setW(0.f);
			btRight.setW(0.f);
			btForward.setW(0.f);


			btVector3 red(1, 0, 0);
			btVector3 blue(0, 0, 1);
			btVector3 green(0, 1, 0);

			debugdraw->drawLine(pos, pos + btUp * factor, green);
			debugdraw->drawLine(pos, pos + btRight * factor, red);
			debugdraw->drawLine(pos, pos + btForward * factor, blue);

		}
	}
}

//========================================================================
//��ȯ���� �浹ü�� �ּҰ����� ������ ���̳� ������Ʈ�� ���� id���� ���������
//���߿� ��������� �޾ƿ� �� �ִ�
//���̳��� ���忡 ��ϵ� ������� arry�� �־��� �� ��ü�� �������� �� ��� ������
//�и��� ������ �ּҸ� id������ ��� 
//�� ����� ������ ���� btCollisionObject*�� ��ȯ�޴� �Լ��� �߰��Ͽ� ����ϸ� ��
//world->getCollisionObjectArray() ����ϸ� ���峻�ο� ��� ��ü�� ��ȯ���� �迭ó�� ����ϸ� ��
//========================================================================
void Physics::AddObject(IShapes& body, size_t* outID, int activation)
{

	bool isDynamic = (body.GetMass() != 0.f);

	btVector3 localInertia(0, 0, 0); //�������Ʈ
	
	if (isDynamic)
		body.GetPointer()->calculateLocalInertia(body.GetMass(), localInertia);

	btDefaultMotionState* fallMotionState = new btDefaultMotionState(body.GetTrans());
	btRigidBody::btRigidBodyConstructionInfo rbInfo(body.GetMass(), fallMotionState, body.GetPointer(), localInertia);

	btRigidBody* fallbody = new btRigidBody(rbInfo);
	size_t id = reinterpret_cast<size_t>(fallbody);

	fallbody->setActivationState(activation);
	fallbody->activate(true);
	
	world->addRigidBody(fallbody);

	contain.insert(make_pair(id, static_cast<btCollisionObject*>(fallbody)));

	if (outID != nullptr)
		*outID = id;
}

void Physics::RemoveObject(size_t id, bool bTemp)
{
	//TODO : �޸������� �Ǵ��� Ȯ���ʿ�
	world->removeCollisionObject(GetCollisionObject(id));
	
	//�����̳ʿ��� ����
	if (bTemp == true)
	{
		unordered_map<size_t, btCollisionObject*>::iterator Iter;
		Iter = contain.find(id);

		contain.erase(Iter);
	}
}

void Physics::Raycast()
{
	if (world)
	{
		world->updateAabbs();
		world->computeOverlappingPairs();

		btVector3 red(1, 0, 0);
		btVector3 blue(0, 0, 1);
		static float up = 0.f;

		///all hits test
		{
			btVector3 from(-30, 1 + up, 0);
			btVector3 to(30, 1, 0);
			btCollisionWorld::AllHitsRayResultCallback allResults(from, to);
			allResults.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;
			///kF_UseGjkConvexRaytest flag is now enabled by default, use the faster but more approximate algorithm
			///allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;
			//allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;

			world->rayTest(from, to, allResults);

			for (int i = 0;i < allResults.m_hitFractions.size();i++)
			{
				btVector3 p = from.lerp(to, allResults.m_hitFractions[i]);
				world->getDebugDrawer()->drawSphere(p, 0.1, red);
				world->getDebugDrawer()->drawLine(p, p + allResults.m_hitNormalWorld[i], red);

			}
		}//all hit

		 //first hit, mouse pick
		{
			
			D3DXVECTOR3 origin, dir, dest;
			GetRay(&origin, &dir);
			dest = origin + dir * 100; // dir * factor

			btVector3 from = btVector3(origin.x, origin.y, origin.z);
			btVector3 to = btVector3(dest.x, dest.y, dest.z);

			btCollisionWorld::ClosestRayResultCallback	closestResults(from, to);
			closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

			world->rayTest(from, to, closestResults);


			if (closestResults.hasHit())
			{
				btVector3 p = from.lerp(to, closestResults.m_closestHitFraction);
				world->getDebugDrawer()->drawSphere(p, 0.1, blue);

				//�ӵ��ο�===========================================

				if (Mouse::Get()->Down(0))
				{
					//��ŷ�ȳ� ����
					pick = closestResults.m_collisionObject;
				}
				if (Mouse::Get()->Press(0))
				{
				}
				else if (Mouse::Get()->Up(0))
				{
				}

			}
			if (Keyboard::Get()->Down(VK_SPACE))
			{
				SetVelocity(GetPickedID(), D3DXVECTOR3(0, 9.8f, 0));
			}

			static D3DXVECTOR3 forward, position;
			static D3DXMATRIX mat;
			auto AngleUpdate = [&]() {
				D3DXQUATERNION q = GetQuaternion(GetPickedID());
				D3DXMatrixRotationQuaternion(&mat, &q);
				D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0, 0, 1.f), &mat);
				D3DXVec3Normalize(&forward, &forward);
			};

			if (Keyboard::Get()->Down(VK_UP))
			{
				AngleUpdate();
			}
			else if (Keyboard::Get()->Press(VK_UP))
			{
				const btRigidBody* cBody = btRigidBody::upcast(GetPickedCollisionObject());
				btRigidBody* body = const_cast<btRigidBody*>(cBody);

				btVector3 temp = body->getLinearVelocity();
				memcpy(&forward.y, &temp.getY(), sizeof(float));

				SetVelocity(GetPickedID(), forward);
			}
			else if (Keyboard::Get()->Down(VK_DOWN))
			{
				AngleUpdate();
			}
			else if (Keyboard::Get()->Press(VK_DOWN))
			{

				SetVelocity(GetPickedID(), -forward);
			}
			if (Keyboard::Get()->Press(VK_RIGHT))
			{
				AngleUpdate();
				float rotation = Math::PI * Time::Delta() * 50.0f;
				SetAngleVelocity(GetPickedID(), D3DXVECTOR3(0, rotation, 0));

			}
			else if (Keyboard::Get()->Press(VK_LEFT))
			{
				AngleUpdate();
				float rotation = -Math::PI * Time::Delta() * 50.0f;
				SetAngleVelocity(GetPickedID(), D3DXVECTOR3(0, rotation, 0));
			}
		}

	}

}

void Physics::GetRay(D3DXVECTOR3 * origin, D3DXVECTOR3 * dir)
{
	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	D3DXVECTOR2 screenSize;
	screenSize.x = (float)desc.Width;
	screenSize.y = (float)desc.Height;

	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	D3DXVECTOR2 point;
	point.x = ((2.0f * mouse.x) / screenSize.x) - 1.0f;
	point.y = (((2.0f * mouse.y) / screenSize.y) - 1.0f) * -1.0f;

	D3DXMATRIX projection;
	projection = camera->GetProjection();
	point.x = point.x / projection._11;
	point.y = point.y / projection._22;

	D3DXMATRIX invView, view;
	view = camera->GetViewMatrix();
	D3DXMatrixInverse(&invView, NULL, &view);
	
	(*dir).x = (point.x * invView._11) + (point.y * invView._21) + invView._31;
	(*dir).y = (point.x * invView._12) + (point.y * invView._22) + invView._32;
	(*dir).z = (point.x * invView._13) + (point.y * invView._23) + invView._33;
	(*origin).x = invView._41;
	(*origin).y = invView._42;
	(*origin).z = invView._43;

	D3DXVec3Normalize(dir, dir);
}

btCollisionObject * Physics::GetCollisionObject(size_t id)
{
	unordered_map<size_t, btCollisionObject*>::iterator Iter;
	Iter = contain.find(id);

	if (contain.end() != Iter)
	{
		return Iter->second;
	}

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	MessageBox(desc.Handle, L"GetCollisionObject", L"��ϵ��� �ʴ� ID", MB_OK);

	return nullptr;
}

D3DXQUATERNION Physics::GetQuaternion(size_t id)
{
	unordered_map<size_t, btCollisionObject*>::iterator Iter;
	Iter = contain.find(id);

	D3DXQUATERNION q;
	if (contain.end() != Iter)
	{

		btTransform transform = (Iter->second)->getWorldTransform();
		btQuaternion btQ = transform.getRotation();
		
		memcpy(&q, &btQ, sizeof(btScalar) * 4);

		return q;
	}

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	MessageBox(desc.Handle, L"GetQuaternion", L"��ϵ��� �ʴ� ID", MB_OK);

	return q;
}

D3DXVECTOR3 Physics::GetRotation(size_t id)
{
	//���ʹϾ��� �ַ� ���°��� ��õ
	return quat_2_euler_d3d(GetQuaternion(id));
}

D3DXVECTOR3 Physics::GetPosition(size_t id)
{
	unordered_map<size_t, btCollisionObject*>::iterator Iter;
	Iter = contain.find(id);

	D3DXVECTOR3 pos;
	if (contain.end() != Iter)
	{

		btTransform transform = (Iter->second)->getWorldTransform();
		btVector3 vec = transform.getOrigin();

		//btVector3 = float * 4  D3DXVECTOR3�� ũ�Ⱑ �ٸ���
		memcpy(&pos, &vec, sizeof(D3DXVECTOR3));

		return pos;
	}

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	MessageBox(desc.Handle, L"GetPosition", L"��ϵ��� �ʴ� ID", MB_OK);

	return pos;
}

D3DXMATRIX Physics::GetMatrix(size_t id)
{
	unordered_map<size_t, btCollisionObject*>::iterator Iter;
	Iter = contain.find(id);

	D3DXMATRIX mat, rotMat;
	D3DXQUATERNION q;
	D3DXVECTOR4 vec;

	if (contain.end() != Iter)
	{
		btTransform transform = (Iter->second)->getWorldTransform();
		btQuaternion btQ = transform.getRotation();
		btVector3 btVec = transform.getOrigin();
		
		memcpy(&q, &btQ, sizeof(btScalar) * 4);
		memcpy(&vec, &btVec, sizeof(D3DXVECTOR3));
		vec.w = 1.0f;

		D3DXMatrixRotationQuaternion(&rotMat, &q);

		memcpy(&mat, &rotMat, sizeof(D3DXVECTOR4) * 3);
		memcpy(&mat._41, &vec, sizeof(D3DXVECTOR4));

		return mat;
	}

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	MessageBox(desc.Handle, L"GetMatrix", L"��ϵ��� �ʴ� ID", MB_OK);

	return mat;
}

float Physics::GetMass(size_t id)
{
	btCollisionObject* obj = const_cast<btCollisionObject*>(GetCollisionObject(id));
	const btRigidBody* cBody = btRigidBody::upcast(obj);
	btRigidBody* body = const_cast<btRigidBody*>(cBody);

	btScalar btMass = 1.f / body->getInvMass();

	float mass;
	memcpy(&mass, &btMass, sizeof(float));

	if (mass < 0.f )
	{
		D3DDesc desc;
		DxRenderer::GetDesc(&desc);
		MessageBox(desc.Handle, L"GetMass", L"������ Ȯ�� (TEST)", MB_OK);
	}

	return mass;
}
//����!! btVector3�� D3DXVECTOR3���� �ϳ� �� ũ�� 
//btVector3.w �⺻���� 0.f  
void Physics::SetVelocity(size_t id, D3DXVECTOR3 value)
{
	unordered_map<size_t, btCollisionObject*>::iterator Iter;
	Iter = contain.find(id);

	if (contain.end() != Iter)
	{
		btVector3 force;
		memcpy(&force, &value, sizeof(D3DXVECTOR3));
		force.setW(0.f);

		btRigidBody* body;
		body = static_cast<btRigidBody*>(Iter->second);

		body->setLinearVelocity(force);
		body->activate(true);

		return;
	}

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	MessageBox(desc.Handle, L"SetVelocity", L"��ϵ��� �ʴ� ID", MB_OK);
}

void Physics::SetPosition(size_t id, D3DXVECTOR3 value)
{
	unordered_map<size_t, btCollisionObject*>::iterator Iter;
	Iter = contain.find(id);

	if (contain.end() != Iter)
	{
		btVector3 pos;
		memcpy(&pos, &value, sizeof(D3DXVECTOR3));
		pos.setW(0.f);

		btRigidBody* body;
		body = static_cast<btRigidBody*>(Iter->second);

		btTransform transform = body->getWorldTransform();
		transform.setOrigin(pos);

		body->setWorldTransform(transform);
		body->activate(true);

		return;
	}

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	MessageBox(desc.Handle, L"SetPosition", L"��ϵ��� �ʴ� ID", MB_OK);
}

void Physics::SetAngleVelocity(size_t id, D3DXVECTOR3 value)
{
	//���ӵ� �ο�
	unordered_map<size_t, btCollisionObject*>::iterator Iter;
	Iter = contain.find(id);

	if (contain.end() != Iter)
	{
		btVector3 angle;
		memcpy(&angle, &value, sizeof(D3DXVECTOR3));
		angle.setW(0.f);

		btRigidBody* body;
		body = static_cast<btRigidBody*>(Iter->second);

		body->setAngularVelocity(angle);
		body->activate(true);

		return;
	}

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	MessageBox(desc.Handle, L"SetAngleVelocity", L"��ϵ��� �ʴ� ID", MB_OK);
}

void Physics::SetQuaternion(size_t id, D3DXQUATERNION value)
{
	unordered_map<size_t, btCollisionObject*>::iterator Iter;
	Iter = contain.find(id);

	if (contain.end() != Iter)
	{
		btQuaternion q;
		memcpy(&q, &value, sizeof(D3DXQUATERNION));

		btRigidBody* body;
		body = static_cast<btRigidBody*>(Iter->second);

		btTransform transform = body->getWorldTransform();
		btQuaternion btQ = transform.getRotation();

		transform.setRotation(q);

		body->setWorldTransform(transform);
		body->activate(true);

		return;
	}

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	MessageBox(desc.Handle, L"SetQuaternion", L"��ϵ��� �ʴ� ID", MB_OK);
}

void Physics::SetMass(size_t id, float m)
{
	
	if (m < FLT_EPSILON) return;

	const btRigidBody* cBody = btRigidBody::upcast(GetCollisionObject(id));
	btRigidBody* body = const_cast<btRigidBody*>(cBody);

	//����
	RemoveObject(id, false);

	//���Ʈ ���
	bool isDynamic = (m != 0.f);

	btVector3 localInertia(0, 0, 0); //�������Ʈ

	if (isDynamic)
	{
		body->getCollisionShape()->calculateLocalInertia(m, localInertia);
		body->setMassProps(m, localInertia);
	}

	//�߰�
	world->addRigidBody(body);

}

//D3DXQUATERNION Physics::GetPickedQuaternion()
//{
//	D3DXQUATERNION q;
//
//	if (pick == nullptr) return q;
//
//	btTransform transform = pick->getWorldTransform();
//	btQuaternion btQ = transform.getRotation();
//
//	memcpy(&q, &btQ, sizeof(btScalar) * 4);
//
//	return q;
//}
//
//D3DXVECTOR3 Physics::GetPickedPosition()
//{
//	D3DXVECTOR3 pos;
//
//	if (pick == nullptr) return pos;
//
//	btTransform transform = pick->getWorldTransform();
//	btVector3 vec = transform.getOrigin();
//
//	//btVector3 = float * 4  D3DXVECTOR3�� ũ�Ⱑ �ٸ���
//	memcpy(&pos, &vec, sizeof(D3DXVECTOR3));
//
//	return pos;
//}
//
//D3DXMATRIX Physics::GetPickedMatrix()
//{
//	D3DXMATRIX mat, rotMat;
//	D3DXQUATERNION q;
//	D3DXVECTOR4 vec;
//
//	if (pick == nullptr) return mat;
//
//
//	btTransform transform = pick->getWorldTransform();
//	btQuaternion btQ = transform.getRotation();
//	btVector3 btVec = transform.getOrigin();
//
//	memcpy(&q, &btQ, sizeof(btScalar) * 4);
//	memcpy(&vec, &btVec, sizeof(D3DXVECTOR3));
//	vec.w = 1.0f;
//
//	D3DXMatrixRotationQuaternion(&rotMat, &q);
//
//	memcpy(&mat, &rotMat, sizeof(D3DXVECTOR4) * 3);
//	memcpy(&mat._41, &vec, sizeof(D3DXVECTOR4));
//
//	return mat;
//}
//
//float Physics::GetPickedMass()
//{
//	return 0.0f;
//}

Physics * Physics::Get()
{
	assert(instance != NULL);

	return instance;
}

void Physics::Create()
{
	assert(instance == NULL);

	instance = new Physics();
}

void Physics::Delete()
{
	SafeDelete(instance);
}
