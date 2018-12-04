#include "stdafx.h"
#include "TestScene.h"

#include "./View/FreeCamera.h"
#include "TestClass.h"

#include "./Utilities/GeometryGenerator.h"
#include "./Utilities/Buffer.h"

#include "./Renders/Font.h"

TestScene::TestScene()
{
	Physics::Create();
	RenderRequest->AddRender("TestSceneUIRender", bind(&TestScene::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("TestSceneRender", bind(&TestScene::Render, this), RenderType::Render);
	RenderRequest->AddRender("TestSceneAlphaRender", bind(&TestScene::AlphaRender, this), RenderType::AlphaRender);

	temp = new TestClass;

	freeCamera = Cameras->AddCamera("FreeCamera", new FreeCamera);


	vertex.assign(4, VertexColor());

	vertex[0].position = D3DXVECTOR3(0, 0, 0);
	vertex[0].color = D3DXCOLOR(1, 1, 1, 1);

	vertex[1].position = D3DXVECTOR3(0, 0, 50);
	vertex[1].color = D3DXCOLOR(1, 1, 1, 1);

	vertex[2].position = D3DXVECTOR3(50, 0, 0);
	vertex[2].color = D3DXCOLOR(1, 1, 1, 1);

	vertex[3].position = D3DXVECTOR3(50, 0, 50);
	vertex[3].color = D3DXCOLOR(1, 1, 1, 1);

	UINT* index = new UINT[6]{ 0,1,2,1,3,2 };

	Buffer::CreateVertexBuffer(&vertexBuffer, &vertex[0], sizeof VertexColor * 4);
	Buffer::CreateIndexBuffer(&indexBuffer, index, 6);

	worldBuffer = new WorldBuffer;
	shader = new Shader(ShaderPath + L"TestBT.hlsl");

	font = new Font(Contents + L"Fonts/ASKII.fnt", Contents + L"Fonts/ASKII_0.png");
}


TestScene::~TestScene()
{
	Release();
}

void TestScene::Init()
{
}

void TestScene::Release()
{
	SafeDelete(temp);
	Physics::Delete();
}

void TestScene::PreUpdate()
{
	
}

void TestScene::Update()
{
	Physics::Get()->Update();
	temp->Update();
	font->Update();
}

void TestScene::PostUpdate()
{
	freeCamera->Update();
}


void TestScene::AlphaRender()
{
	font->Render();

}

void TestScene::PreRender()
{
	//camera정보를 deferred에게 언팩킹시에 필요한 정보를 보낸다
	RenderRequest->SetUnPackGBufferProp(freeCamera->GetViewMatrix(), freeCamera->GetProjection());

}

void TestScene::Render()
{
	freeCamera->Render();

	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	worldBuffer->SetMatrix(mat);

	worldBuffer->SetVSBuffer(1);

	
	shader->Render();

	DeviceContext->DrawIndexed(6, 0, 0);



	temp->Render();
	Physics::Get()->Render();

}

void TestScene::UIRender()
{
	temp->PostRender();
	Physics::Get()->ImguiRender();
}

