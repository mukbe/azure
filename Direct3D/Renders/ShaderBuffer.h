#pragma once
#include "stdafx.h"

class ShaderBuffer
{
private:
	friend class BufferManager;
public:
	void SetVSBuffer(UINT slot)
	{
		MapData(data, dataSize);

		DeviceContext->VSSetConstantBuffers(slot, 1, &buffer);
	}

	void SetPSBuffer(UINT slot)
	{
		MapData(data, dataSize);

		DeviceContext->PSSetConstantBuffers(slot, 1, &buffer);
	}

	void SetHSBuffer(UINT slot)
	{
		MapData(data, dataSize);

		DeviceContext->HSSetConstantBuffers(slot, 1, &buffer);
	}

	void SetDSBuffer(UINT slot)
	{
		MapData(data, dataSize);

		DeviceContext->DSSetConstantBuffers(slot, 1, &buffer);
	}

	void SetGSBuffer(UINT slot)
	{
		MapData(data, dataSize);

		DeviceContext->GSSetConstantBuffers(slot, 1, &buffer);
	}

	void SetCSBuffer(UINT slot)
	{
		MapData(data, dataSize);

		DeviceContext->CSSetConstantBuffers(slot, 1, &buffer);
	}

protected:
	ShaderBuffer(void* data, UINT dataSize)
		: data(data), dataSize(dataSize)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = dataSize;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		HRESULT hr = DxRenderer::Get()->GetDevice()->CreateBuffer(&desc, NULL, &buffer);
		assert(SUCCEEDED(hr));
	}

	virtual ~ShaderBuffer()
	{
		SafeRelease(buffer);
	}

private:
	void MapData(void* data, UINT dataSize)
	{
		D3D11_MAPPED_SUBRESOURCE subResource;

		HRESULT hr = DeviceContext->Map
		(
			buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
		);

		memcpy(subResource.pData, data, dataSize);

		DeviceContext->Unmap(buffer, 0);
	}

	D3D11_BUFFER_DESC desc;
	ID3D11Buffer* buffer;

	void* data;
	UINT dataSize;
};

#define ShaderBuffer_Mecro(className) public : static string GetCode(){return typeid(className).name(); }