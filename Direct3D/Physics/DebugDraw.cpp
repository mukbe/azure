#include "stdafx.h"
#include "DebugDraw.h"

#include "./Renders/WorldBuffer.h"


DebugDraw::DebugDraw()
{
	//쉐이더는 기본 VertexColor면 충분하다 월드변환만 해주고 들어온 색으로반환하면 끝
	shader = new Shader(ShaderPath + L"001_GBuffer.hlsl", Shader::ShaderType::Default, "GizmoDeferred");
	worldBuffer = Buffers->FindShaderBuffer<WorldBuffer>();

	//월드버퍼는 단위행렬로 두고 쓰질 않는다
	//랜더링하기 바로 전에 버텍스들을 할당하고 랜더링 후에 바로 해제하기 때문에 
	//월드변환을 하지않고 버텍스좌표를 가져다가 쓴다
	//맘에 안들면 바꾸시오
	
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

	//UpdateSubresource를 사용하지 않는 이유
	//업데이트를 돌면서 컨테이너에 버텍스 정보를 담아두고 그걸 이용해서 그리는것인데
	//Init에서 버텍스를 할당해버리면 업데이트도중에 객체를 사라지거나 하면 오류가 생겨버린다
	//이를 간단히 해결하기위함. 맘에 내키지는 않지만 디버깅이니...
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
		//버텍스만 가지고 그려줌
		DeviceContext->Draw(aLine.size() * 2, 0);
	}
	DeviceContext->OMSetDepthStencilState(onState, 1);


	//diffuse 휜색
	//D3D::GetDC()->Draw(aLine.size() * 2, 0);


	//갱신하기 위해서 컨테이너를 비운다
	aLine.clear();
	SafeRelease(vertexBuffer);
}
