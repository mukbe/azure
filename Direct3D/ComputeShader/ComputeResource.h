#pragma once
class IComputeResource
{
public:
	IComputeResource();
	virtual ~IComputeResource();

	virtual void BindCSShaderResourceView(int slot);
	virtual void ReleaseCSshaderResorceView(int slot);

	virtual void BindPSShaderResourceView(int slot);
	virtual void ReleaseShaderResourceView(int slot);

	virtual void BindDSShaderResourceView(int slot);
	virtual void ReleaseDSshaderResorceView(int slot);

	virtual void UpdateSRV() {};


	virtual void BindResource(int slot, UINT* init = nullptr);
	virtual void ReleaseResource(int slot);

	virtual ID3D11ShaderResourceView* GetSRV() { return srv; }

protected:
	ID3D11ShaderResourceView *srv;
	ID3D11UnorderedAccessView *uav;
};

class CResource1D : public IComputeResource
{
public:
	CResource1D(UINT elementSize, UINT count, void* pInitData);
	virtual~CResource1D();
	
	virtual void GetDatas(void* datas);

	ID3D11Buffer* GetReadBuffer() { return readBuffer; }
	ID3D11Buffer* GetRWBuffer() { return rwBuffer; }
private:
	void CreateBufferForGPU(UINT elementSize, UINT count, void * pInitData, ID3D11Buffer* buffer);
	void CreateSRV();
	void CreateUAV();

	ID3D11Buffer * rwBuffer;
	ID3D11Buffer* readBuffer;
	UINT byteWidth;
};

class CAppendResource1D : public IComputeResource
{
public:
	CAppendResource1D(UINT elementSize, UINT count, void* pInitData);
	virtual~CAppendResource1D();

	virtual void GetDatas(void* datas);
	void SetDatas(void* datas);
	ID3D11UnorderedAccessView* GetUAV() { return uav; }

private:
	void CreateBufferForGPU(UINT elementSize, UINT count, void * pInitData, ID3D11Buffer* buffer);
	void CreateSRV();
	void CreateUAV();

	ID3D11Buffer * rwBuffer;
	ID3D11Buffer* readBuffer;
	UINT byteWidth;
	UINT counts;
};

class CResource2D : public IComputeResource
{
public:
	CResource2D(UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT, void * pInitData = nullptr);
	virtual~CResource2D();

	virtual void GetDatas(vector<vector<D3DXCOLOR>>& colors);	

	void SavePNG(wstring path);

	virtual void UpdateSRV();

private:
	void CreateBufferForGPU(UINT width, UINT height, void * pInitData, ID3D11Texture2D* buffer);
	void CreateSRV();
	void CreateUAV();

	ID3D11Texture2D * rwBuffer;
	ID3D11Texture2D* readBuffer;
	DXGI_FORMAT format;
	UINT byteWidth;

};

class CResource3D : public IComputeResource
{
public:
	CResource3D(UINT width, UINT height, UINT depth, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT, void * pInitData = nullptr);
	virtual~CResource3D();

	//virtual void GetDatas(vector<vector<D3DXCOLOR>>& colors);


private:
	void CreateBufferForGPU(UINT width, UINT height, UINT depth, void * pInitData, ID3D11Texture3D* buffer);
	void CreateSRV();
	void CreateUAV();

	ID3D11Texture3D * rwBuffer;
	ID3D11Texture3D* readBuffer;
	DXGI_FORMAT format;
	UINT byteWidth;
	UINT depth;
};

class CResourceIndirect : public IComputeResource
{
public:
	CResourceIndirect(void* pInitData);
	virtual~CResourceIndirect();


	ID3D11Buffer* GetReadBuffer() { return readBuffer; }
	ID3D11Buffer* GetRWBuffer() { return rwBuffer; }
	ID3D11Buffer* GetIndirectBuffer() { return rwBuffer; }

	void GetDatas(void* datas);

private:
	void CreateBufferForGPU(void * pInitData, ID3D11Buffer* buffer);
	void CreateSRV();
	void CreateUAV();

	ID3D11Buffer * rwBuffer;
	ID3D11Buffer* readBuffer;
};
