#include "stdafx.h"
#include "TestClass.h"

TestClass::TestClass()
{

	shader = new Shader(ShaderPath + L"TestBT.hlsl");
	worldbuffer = new WorldBuffer();


	VertexColor v[8];
	v[0].position = D3DXVECTOR3(-1, 1, -1);
	v[1].position = D3DXVECTOR3(1, 1, -1);
	v[2].position = D3DXVECTOR3(1, -1, -1);
	v[3].position = D3DXVECTOR3(-1, -1, -1);

	v[4].position = D3DXVECTOR3(-1, 1, 1);
	v[5].position = D3DXVECTOR3(1, 1, 1);
	v[6].position = D3DXVECTOR3(1, -1, 1);
	v[7].position = D3DXVECTOR3(-1, -1, 1);

	D3DXCOLOR color(1, 0, 1, 1);
	for (int i = 0; i < 8; i++)
		v[i].color = color;

	lines.push_back(Line(v[0], v[1]));
	lines.push_back(Line(v[1], v[2]));
	lines.push_back(Line(v[2], v[3]));
	lines.push_back(Line(v[3], v[0]));

	lines.push_back(Line(v[0], v[4]));
	lines.push_back(Line(v[1], v[5]));
	lines.push_back(Line(v[2], v[6]));
	lines.push_back(Line(v[3], v[7]));

	lines.push_back(Line(v[4], v[5]));
	lines.push_back(Line(v[5], v[6]));
	lines.push_back(Line(v[6], v[7]));
	lines.push_back(Line(v[7], v[4]));


	//VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(Line) * lines.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = lines.data();

		HRESULT hr;
		hr = Device->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}
	ShapeTerrain test(51, 51, nullptr, 1);
	test.SetPosition(D3DXVECTOR3(25, 0, 25));
	Physics::Get()->AddObject(test);


	//AddObject(Shpae, ID = nullptr)
	//나중에 물체의 행렬이 필요하다면 id값을 저장
	//ShapePlane test1;
	//test1.SetRotation(D3DXVECTOR3(0, 0, 0 ));
	//Physics::Get()->AddObject(test1);

	ShapeBox box1;
	box1.SetPosition(D3DXVECTOR3(15, 120, 15));
	Physics::Get()->AddObject(box1, &testID1);

	ShapeBox box2;
	box2.SetPosition(D3DXVECTOR3(1, 0, 1));
	box2.SetRotation(D3DXVECTOR3(0, 0, 0));
	box2.SetMass(100);
	Physics::Get()->AddObject(box2, &testID2);

	Physics::Get()->GetMass(testID2);


}

TestClass::~TestClass()
{

}

void TestClass::Update()
{
	//GetCollisionObject로 직접 가져올 수 있다
	matWorld = Physics::Get()->GetMatrix(testID1);
	matWorld2 = Physics::Get()->GetMatrix(testID2);
}

void TestClass::Render()
{
	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	{//box1
		worldbuffer->SetMatrix(matWorld);
		worldbuffer->SetVSBuffer(1);
		shader->Render();

		DeviceContext->Draw(lines.size() * 2, 0);
	}

	{//box2
		worldbuffer->SetMatrix(matWorld2);
		worldbuffer->SetVSBuffer(1);
		DeviceContext->Draw(lines.size() * 2, 0);
	}

}

void TestClass::PostRender()
{
	ImGui::Begin("Test box");
	{
		static float pos[3] = { 0, 0, 0 };
		static float rot[3] = { 0, 0, 0 };
		static float speed[3] = { 0, 0, 0 };
		static float angleSpeed[3] = { 0, 0, 0 };
		static float mass = 0.f;

		ImGuiInputTextFlags flag = 0;
		flag |= ImGuiInputTextFlags_EnterReturnsTrue;

		size_t id = Physics::Get()->GetPickedID();
		if (id != 0)
		{

			//memcpy(&vec, pos, sizeof(float) * 3);
			//memcpy(&vec, pos, sizeof(float) * 3);
			//memcpy(&vec, pos, sizeof(float) * 3);
			//memcpy(&vec, pos, sizeof(float) * 3);

			//SetPosition
			{
				D3DXVECTOR3 position = Physics::Get()->GetPosition(id);
				memcpy(&pos, &position, sizeof(float) * 3);

				if (ImGui::InputFloat3("SetPosition", pos, 1, flag))
				{
					D3DXVECTOR3 vec;
					memcpy(&vec, pos, sizeof(float) * 3);
					Physics::Get()->SetPosition(id, vec);

					ZeroMemory(pos, sizeof(float) * 3);
				}
			}

			//SetRotation
			{
				D3DXVECTOR3 rotation = Physics::Get()->GetRotation(id);
				float toG = 180.f / (float)D3DX_PI;
				rotation = rotation * toG;
				memcpy(&rot, &rotation, sizeof(float) * 3);

				if (ImGui::InputFloat3("SetRotation", rot, 1, flag))
				{

					D3DXQUATERNION q;
					float toRad = (float)D3DX_PI / 180.0f;
					D3DXQuaternionRotationYawPitchRoll
					(
						&q
						, rot[1] * toRad
						, rot[0] * toRad
						, rot[2] * toRad
					);

					Physics::Get()->SetQuaternion(id, q);

					ZeroMemory(rot, sizeof(float) * 3);
				}
			}




			if (ImGui::InputFloat3("SetVelocity", speed, 1, flag))
			{
				D3DXVECTOR3 vec;
				memcpy(&vec, speed, sizeof(float) * 3);
				Physics::Get()->SetVelocity(id, vec);

				ZeroMemory(speed, sizeof(float) * 3);
			}

			if (ImGui::InputFloat3("SetAngleVelocity", angleSpeed, 1, flag))
			{
				D3DXVECTOR3 vec;
				float toRad = (float)D3DX_PI / 180.0f;

				memcpy(&vec, angleSpeed, sizeof(float) * 3);
				vec = vec * toRad;

				Physics::Get()->SetAngleVelocity(id, vec);

				ZeroMemory(angleSpeed, sizeof(float) * 3);
			}

			//SetMass
			{
				float m = Physics::Get()->GetMass(id);
				mass = m;

				if (ImGui::InputFloat("SetMass", &mass, 0.1f, 0.5f, 1, flag))
				{
					Physics::Get()->SetMass(id, mass);

					mass = 0.f;
				}
			}
		}
	}
	ImGui::End();

}
