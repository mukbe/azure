#include "stdafx.h"
#include "Gizmo.h"

#include "./Renders/Shader.h"
#include "./Utilities/Buffer.h"
#include "./Renders/WorldBuffer.h"
#include "./Renders/Shader.h"
#include "./Bounding/BoundingBox.h"

SingletonCpp(Gizmo)

void Gizmo::CreateVertex()
{
	VertexColor* vertex = new VertexColor[2];

	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(VertexColor) * 2;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = vertex;
	HRESULT hr = Device->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));

	circleVertices = new VertexColor[CIRCLEGIZMO_SEGMENTS + 1];

	desc = { 0 };
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(VertexColor) * (CIRCLEGIZMO_SEGMENTS + 1);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	data = { 0 };
	data.pSysMem = circleVertices;
	hr = Device->CreateBuffer(&desc, &data, &circleVertexBuffer);
	assert(SUCCEEDED(hr));

	SafeDeleteArray(vertex);
}

void Gizmo::CreateIndex()
{
	indexCount = 2;
	UINT* index = new UINT[indexCount];

	D3D11_BUFFER_DESC desc = { 0 };
	index[0] = 0;
	index[1] = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = index;
	HRESULT hr = Device->CreateBuffer
	(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));

	UINT* circleIndex = new UINT[CIRCLEGIZMO_SEGMENTS + 1];
	for (int i = 0; i <= CIRCLEGIZMO_SEGMENTS; i++)
		circleIndex[i] = i;

	desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * (CIRCLEGIZMO_SEGMENTS + 1);
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	data = { 0 };
	data.pSysMem = circleIndex;
	hr = Device->CreateBuffer
	(&desc, &data, &circleIndexBuffer);
	assert(SUCCEEDED(hr));

	SafeDeleteArray(index);
	SafeDeleteArray(circleIndex);
}

Gizmo::Gizmo()
{
	CreateIndex();
	CreateVertex();

	float intervalAngle = (D3DX_PI * 2.0f) / CIRCLEGIZMO_SEGMENTS;

	for (int i = 0; i <= CIRCLEGIZMO_SEGMENTS; i++)
	{
		float angle = i * intervalAngle;
		circlPosition[i] = D3DXVECTOR3(cos(angle), sin(angle), 0);
	}

	worldBuffer = Buffers->FindShaderBuffer<WorldBuffer>();
	
	shader = new Shader(ShaderPath + L"001_GBuffer.hlsl", Shader::ShaderType::Default,"GizmoDeferred");
}


Gizmo::~Gizmo()
{
	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);
	SafeDelete(shader);

	SafeDeleteArray(circleVertices);
}

void Gizmo::Line(const D3DXVECTOR3 startPos, const D3DXVECTOR3 endPos, const D3DXCOLOR color)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = DeviceContext->Map
	(
		vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource
	);
	{
		assert(SUCCEEDED(hr));
		VertexColor* buffer = (VertexColor*)mappedResource.pData;
		buffer[0].position = startPos;
		buffer[1].position = endPos;
		buffer[0].color = color;
		buffer[1].color = color;
	}
	DeviceContext->Unmap(vertexBuffer, NULL);

	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	worldBuffer->SetMatrix(IdentityMatrix);
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	DeviceContext->DrawIndexed(indexCount, 0, 0);
}

void Gizmo::Circle(D3DXVECTOR3 center, float radius, D3DXVECTOR3 axis, D3DXCOLOR color)
{
	//circle에 대한 행렬준비
	D3DXVECTOR3 forward = axis;
	D3DXVECTOR3 right;
	D3DXVECTOR3 up(0, 1, 0);

	if (forward == up)
		up = D3DXVECTOR3(0, 0, -1);
	else if (forward == D3DXVECTOR3(0, -1, 0))
		up = D3DXVECTOR3(0, 0, 1);

	D3DXVec3Cross(&right, &up, &forward);
	D3DXVec3Cross(&up, &forward, &right);

	D3DXVec3Normalize(&forward, &forward);
	D3DXVec3Normalize(&right, &right);
	D3DXVec3Normalize(&up, &up);

	forward = forward * radius;
	right = right * radius;
	up = up * radius;

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);

	memcpy(&matWorld._11, right, sizeof(D3DXVECTOR3));
	memcpy(&matWorld._21, up, sizeof(D3DXVECTOR3));
	memcpy(&matWorld._31, forward, sizeof(D3DXVECTOR3));
	memcpy(&matWorld._41, center, sizeof(D3DXVECTOR3));
	
	for (int i = 0; i <= CIRCLEGIZMO_SEGMENTS; i++)
	{
		circleVertices[i].position = circlPosition[i];
		circleVertices[i].color = color;
	}

	Buffer::UpdateBuffer(&circleVertexBuffer, circleVertices, sizeof VertexColor *(CIRCLEGIZMO_SEGMENTS + 1));

	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &circleVertexBuffer, &stride, &offset);
	DeviceContext->IASetIndexBuffer(circleIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	worldBuffer->SetMatrix(matWorld);
	worldBuffer->SetVSBuffer(1);
	shader->Render();
	DeviceContext->DrawIndexed((CIRCLEGIZMO_SEGMENTS + 1), 0, 0);
}

void Gizmo::Quad(const D3DXVECTOR3 centerPos,const float width,const float height, const D3DXVECTOR3 normal, D3DXCOLOR color)
{
	/* 0 1
	   3 2
	*/ 
	D3DXVECTOR3 corners[4];
	

}

void Gizmo::WireSphere(D3DXVECTOR3 center, float radius, D3DXCOLOR color)
{
	this->Circle(center, radius, D3DXVECTOR3(1, 0, 0), color);
	this->Circle(center, radius, D3DXVECTOR3(0, 1, 0), color);
	this->Circle(center, radius, D3DXVECTOR3(0, 0, 1), color);
}

void Gizmo::AABB(const D3DXVECTOR3 minPos, const D3DXVECTOR3 maxPos, const D3DXCOLOR color)
{
	this->Line(D3DXVECTOR3(minPos.x, minPos.y, minPos.z), D3DXVECTOR3(maxPos.x, minPos.y, minPos.z), color);
	this->Line(D3DXVECTOR3(maxPos.x, minPos.y, minPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z), color);
	this->Line(D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z), D3DXVECTOR3(minPos.x, maxPos.y, minPos.z), color);
	this->Line(D3DXVECTOR3(minPos.x, maxPos.y, minPos.z), D3DXVECTOR3(minPos.x, minPos.y, minPos.z), color);

	this->Line(D3DXVECTOR3(minPos.x, minPos.y, maxPos.z), D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z), color);
	this->Line(D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z), color);
	this->Line(D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z), D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z), color);
	this->Line(D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z), D3DXVECTOR3(minPos.x, minPos.y, maxPos.z), color);

	this->Line(D3DXVECTOR3(minPos.x, minPos.y, minPos.z), D3DXVECTOR3(minPos.x, minPos.y, maxPos.z), color);
	this->Line(D3DXVECTOR3(maxPos.x, minPos.y, minPos.z), D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z), color);
	this->Line(D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z), color);
	this->Line(D3DXVECTOR3(minPos.x, maxPos.y, minPos.z), D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z), color);
}

void Gizmo::OBB(vector<D3DXVECTOR3>& corners, const D3DXCOLOR color)
{
	//   0-------1
	//  /|      /|
	// 4-------5 |
	// | 3-----|-2
	// |/      |/
	// 7-------6 

	this->Line(corners[0], corners[1], color);
	this->Line(corners[1], corners[2], color);
	this->Line(corners[2], corners[3], color);
	this->Line(corners[3], corners[0], color);

	this->Line(corners[4], corners[5], color);
	this->Line(corners[5], corners[6], color);
	this->Line(corners[6], corners[7], color);
	this->Line(corners[7], corners[4], color);

	this->Line(corners[0], corners[4], color);
	this->Line(corners[1], corners[5], color);
	this->Line(corners[3], corners[7], color);
	this->Line(corners[2], corners[6], color);

	corners.clear();
}

void Gizmo::Cube(D3DXVECTOR3 center, D3DXVECTOR3 forward, D3DXVECTOR3 up,D3DXCOLOR color)
{
	//   0-------1
	//  /|      /|
	// 4-------5 |
	// | 3-----|-2
	// |/      |/
	// 7-------6 

	vector<D3DXVECTOR3> corners;
	corners.assign(8, D3DXVECTOR3());

	D3DXVECTOR3 right;
	D3DXVec3Cross(&right, &forward, &up);
	D3DXVec3Normalize(&right, &right);
	D3DXVec3Normalize(&up, &up);
	D3DXVec3Normalize(&forward, &forward);

	corners[0] = center + (-right + up + forward);
	corners[1] = center + (right + up + forward); //D3DXVECTOR3(0.5f, 0.5f, 0.5f);
	corners[2] = center + (right - up + forward);//D3DXVECTOR3(0.5f, -0.5f, 0.5f);
	corners[3] = center + (-right - up + forward);// D3DXVECTOR3(-0.5f, -0.5f, 0.5f);
	corners[4] = center + (-right + up - forward); //D3DXVECTOR3(-0.5f, 0.5f, -0.5f);
	corners[5] = center + (right + up - forward );//D3DXVECTOR3(0.5f, 0.5f, -0.5f);
	corners[6] = center + (right - up - forward );//D3DXVECTOR3(0.5f, -0.5f, -0.5f);
	corners[7] = center + (-right - up - forward);//D3DXVECTOR3(-0.5f, -0.5f, -0.5f);

	this->OBB(corners, color);
	//this->AABB(corners[7], corners[1], color);
}

void Gizmo::LocalGizmo(D3DXVECTOR3 center, float radius, D3DXVECTOR3 right, D3DXVECTOR3 up, D3DXVECTOR3 forward,
	D3DXCOLOR rightColor, D3DXCOLOR upColor,D3DXCOLOR forwardColor)
{
	float axisLineLength = radius;
	//x축 라인 
	D3DXVECTOR3 xEnd = center + right * radius;

	D3DXVECTOR3 yEnd = center + up * radius;

	D3DXVECTOR3 zEnd = center + forward * radius;

	Line(center, xEnd, rightColor);
	Line(center, yEnd, upColor);
	Line(center, zEnd,forwardColor);

	this->Cube(xEnd,right,up,rightColor);
	this->Cube(yEnd, up,forward, upColor);
	this->Cube(zEnd, forward,right,forwardColor);
	
}
