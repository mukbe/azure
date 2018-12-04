#include "stdafx.h"
#include "Font.h"
#include "./View/OrthoWindow.h"
#include "./View/FreeCamera.h"

#include <fstream>
#include <sstream>

Font::Font(wstring file, wstring texture)
	: test("dhsysy dhsysy dhsysy")
	, testLocation(0, 0)
	, bRemap(true)
{
	shader = new Shader(ShaderPath + L"Font.hlsl");
	buffer = new Buffer;

	D3DXMatrixIdentity(&view);
	D3DXMatrixIdentity(&world);
	{
		D3DDesc desc;
		pRenderer->GetDesc(&desc);
		
		//¿ÞÂÊÀ§°¡ 0,0
		ortho = new OrthoWindow(D3DXVECTOR2(-WinSizeX / 2, WinSizeY / 2), D3DXVECTOR2(WinSizeX, WinSizeY));
	}

	CreateBuffer();

	Parse(file, texture);

	{
		D3D11_BLEND_DESC desc;
		States::GetBlendDesc(&desc);
		States::CreateBlend(&desc, &blendState[0]);

		desc.RenderTarget[0].BlendEnable = true;
		States::CreateBlend(&desc, &blendState[1]);
	}

	D3DX11_IMAGE_LOAD_INFO info;
	info.Width = fontDesc.Width;
	info.Height = fontDesc.Height;

	HRESULT hr = D3DX11CreateShaderResourceViewFromFile
	(
		Device,texture.c_str(),&info,nullptr,&srv,nullptr
	);
	assert(SUCCEEDED(hr));
}


Font::~Font()
{

	SafeRelease(blendState[0]);
	SafeRelease(blendState[1]);

	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);

	SafeDelete(vertices);
	SafeDelete(indices);

	SafeDelete(ortho);
	SafeDelete(shader);
}

void Font::Update()
{
	if (bRemap)
	{
		bRemap = false;

		for (size_t i = 0; i < test.size(); i++)
		{
			UINT id = test[i];

			float CharX = (float)fontDesc.desc[id].X;
			float CharY = (float)fontDesc.desc[id].Y;
			float Width = (float)fontDesc.desc[id].Width;
			float Height = (float)fontDesc.desc[id].Height;
			float OffsetX = fontDesc.desc[id].OffsetX;
			float OffsetY = fontDesc.desc[id].OffsetY;

			//lower left
			vertices[i * 4 + 0].position.x = testLocation.x + OffsetX;
			vertices[i * 4 + 0].position.y = testLocation.y + Height + OffsetY;
			vertices[i * 4 + 0].uv.x = CharX / fontDesc.Width;
			vertices[i * 4 + 0].uv.y = (CharY + Height) / fontDesc.Height;

			//upper left
			vertices[i * 4 + 1].position.x = testLocation.x + OffsetX;
			vertices[i * 4 + 1].position.y = testLocation.y + OffsetY;
			vertices[i * 4 + 1].uv.x = CharX / fontDesc.Width;
			vertices[i * 4 + 1].uv.y = CharY / fontDesc.Height;

			//lower right
			vertices[i * 4 + 2].position.x = testLocation.x + Width + OffsetX;
			vertices[i * 4 + 2].position.y = testLocation.y + Height + OffsetY;
			vertices[i * 4 + 2].uv.x = (CharX + Width) / fontDesc.Width;
			vertices[i * 4 + 2].uv.y = (CharY + Height) / fontDesc.Height;

			//upper right
			vertices[i * 4 + 3].position.x = testLocation.x + Width + OffsetX;
			vertices[i * 4 + 3].position.y = testLocation.y + OffsetY;
			vertices[i * 4 + 3].uv.x = (CharX + Width) / fontDesc.Width;
			vertices[i * 4 + 3].uv.y = CharY / fontDesc.Height;

			testLocation.x += fontDesc.desc[id].AdvanceX;

		}

		D3D11_MAPPED_SUBRESOURCE subResource;
		HRESULT he = DeviceContext->Map
		(
			vertexBuffer, 0 , D3D11_MAP_WRITE_DISCARD, 0 ,&subResource
		);
		memcpy(subResource.pData, vertices, sizeof(VertexTexture)* MaxSize * 4);
		DeviceContext->Unmap(vertexBuffer,0);
	}
}

void Font::Render()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	D3DXMATRIX projection;
	ortho->GetMatrix(&projection);
	//view = Cameras->FindCamera("FreeCamera")->GetViewMatrix();
	buffer->SetProjection(world, view, projection);

	buffer->SetVSBuffer(11);

	DeviceContext->PSSetShaderResources(5, 1, &srv);

	shader->Render();

	States::SetDepthStencil(States::DepthStencilStates::DEPTH_OFF);
	DeviceContext->OMSetBlendState(blendState[1], NULL, 0xFF);
	DeviceContext->DrawIndexed(test.size() * 6, 0, 0);
	DeviceContext->OMSetBlendState(blendState[0], NULL, 0xFF);
	States::SetDepthStencil(States::DepthStencilStates::DEPTH_ON);

}

void Font::CreateBuffer()
{
	vertices = new VertexTexture[MaxSize * 4];
	for (UINT i = 0; i < MaxSize; i++)
	{
		vertices[i * 4 + 0].position = D3DXVECTOR3(0, 0, 0);
		vertices[i * 4 + 1].position = D3DXVECTOR3(0, 0, 0);
		vertices[i * 4 + 2].position = D3DXVECTOR3(0, 0, 0);
		vertices[i * 4 + 3].position = D3DXVECTOR3(0, 0, 0);

		vertices[i * 4 + 0].uv = D3DXVECTOR2(0, 1);
		vertices[i * 4 + 1].uv = D3DXVECTOR2(0, 0);
		vertices[i * 4 + 2].uv = D3DXVECTOR2(1, 1);
		vertices[i * 4 + 3].uv = D3DXVECTOR2(1, 0);
	}

	indices = new UINT[MaxSize * 6];
	for (UINT i = 0; i < MaxSize; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;

		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 1;
		indices[i * 6 + 5] = i * 4 + 3;
	}

	{//vertexbuffer
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(VertexTexture) * MaxSize * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr;
		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		hr = Device->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	{//indexbuffer
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.ByteWidth = sizeof(UINT) * MaxSize * 6;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		HRESULT hr;
		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		hr = Device->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void Font::Parse(wstring file, wstring texture)
{
	size_t count;
	string line, read, key, value;


	ifstream stream;
	stream.open(file);
	{
		while (stream.eof() == false)
		{
			stringstream lineStream;
			getline(stream, line);
			lineStream << line;

			lineStream >> read;

			if (read == "common")
			{
				while (lineStream.eof() == false)
				{
					lineStream >> read;

					count = read.find('=');
					key = read.substr(0, count);
					value = read.substr(count + 1);

					stringstream convert;

					convert << value;
					if (key == "lineHeight") convert >> fontDesc.LindeHeight;
					else if (key == "base") convert >> fontDesc.Base;
					else if (key == "scaleW") convert >> fontDesc.Width;
					else if (key == "scaleH") convert >> fontDesc.Height;
					else if (key == "pages") convert >> fontDesc.PageCount;

				}//while(lineStream)
			}
			else if (read == "char")
			{
				USHORT id = 0;
				while (lineStream.eof() == false)
				{
					lineStream >> read;

					count = read.find('=');
					key = read.substr(0, count);
					value = read.substr(count + 1);

					stringstream convert;

					convert << value;
					if (key == "id") convert >> id;
					else if (key == "x") convert >> fontDesc.desc[id].X;
					else if (key == "y") convert >> fontDesc.desc[id].Y;
					else if (key == "width") convert >> fontDesc.desc[id].Width;
					else if (key == "height") convert >> fontDesc.desc[id].Height;
					else if (key == "xoffset") convert >> fontDesc.desc[id].OffsetX;
					else if (key == "yoffset") convert >> fontDesc.desc[id].OffsetY;
					else if (key == "xadvance") convert >> fontDesc.desc[id].AdvanceX;
					else if (key == "page") convert >> fontDesc.desc[id].Page;

				}//while(lineStream)
			}
		}
	}
	stream.close();
}
