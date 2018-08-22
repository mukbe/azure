#include "stdafx.h"
#include "DebugDraw.h"

#include "./Renders/WorldBuffer.h"


DebugDraw::DebugDraw()
{
	//���̴��� �⺻ VertexColor�� ����ϴ� ���庯ȯ�� ���ְ� ���� �����ι�ȯ�ϸ� ��
	shader = new Shader(ShaderPath + L"001_GBuffer.hlsl", Shader::ShaderType::Default, "GizmoDeferred");
	worldBuffer = Buffers->FindShaderBuffer<WorldBuffer>();

	//������۴� ������ķ� �ΰ� ���� �ʴ´�
	//�������ϱ� �ٷ� ���� ���ؽ����� �Ҵ��ϰ� ������ �Ŀ� �ٷ� �����ϱ� ������ 
	//���庯ȯ�� �����ʰ� ���ؽ���ǥ�� �����ٰ� ����
	//���� �ȵ�� �ٲٽÿ�
	
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		States::GetDepthStencilDesc(&desc);
		States::CreateDepthStencil(&desc, &onState);

		desc.DepthEnable = !desc.DepthEnable;

		States::CreateDepthStencil(&desc, &offState);
	}


}


DebugDraw::~DebugDraw()
{
	SafeDelete(worldBuffer);
	SafeDelete(shader);
}

void DebugDraw::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & fromColor, const btVector3 & toColor)
{
	if (aLine.size() >= MAX_LINE)return;
	aLine.push_back(Line(from, to, fromColor, toColor));

}

void DebugDraw::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color)
{
	drawLine(from, to, color, color);
}

void DebugDraw::DrawLine()
{
	if (aLine.size() == 0)return;

	//UpdateSubresource�� ������� �ʴ� ����
	//������Ʈ�� ���鼭 �����̳ʿ� ���ؽ� ������ ��Ƶΰ� �װ� �̿��ؼ� �׸��°��ε�
	//Init���� ���ؽ��� �Ҵ��ع����� ������Ʈ���߿� ��ü�� ������ų� �ϸ� ������ ���ܹ�����
	//�̸� ������ �ذ��ϱ�����. ���� ��Ű���� ������ ������̴�...
	HRESULT hr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(Line) * aLine.size();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = aLine.data();

	hr = Device->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	worldBuffer->SetMatrix(mat);
	worldBuffer->SetVSBuffer(1);


	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	shader->Render();

	DeviceContext->OMSetDepthStencilState(offState, 1);
	{
		//���ؽ��� ������ �׷���
		DeviceContext->Draw(aLine.size() * 2, 0);
	}
	DeviceContext->OMSetDepthStencilState(onState, 1);


	//diffuse �ػ�
	//D3D::GetDC()->Draw(aLine.size() * 2, 0);


	//�����ϱ� ���ؼ� �����̳ʸ� ����
	aLine.clear();
	SafeRelease(vertexBuffer);
}
