#pragma once

struct D3DDesc
{
	wstring AppName;
	HINSTANCE Instance;
	HWND Handle;
	float Width;
	float Height;
	bool bVsync;
	bool bFullScreen;
};

class DxRenderer
{
	SingletonHeader(DxRenderer)
public:
	static void GetDesc(D3DDesc* desc)
	{
		*desc = d3dDesc;
	}

	static void SetDesc(D3DDesc& desc)
	{
		d3dDesc = desc;
	}

private:
	static D3DDesc  d3dDesc;

	ID3D11Device* pD3dDevice;
	ID3D11DeviceContext* pD3dContext;

	D3D_DRIVER_TYPE mDriverType;
	D3D_FEATURE_LEVEL mFeatureLevel;

	ID3D11Texture2D* pDepthStencilBuffer;
	ID3D11RenderTargetView* pRenderTargetView;
	ID3D11DepthStencilView* pDepthStencilView;
	D3D11_VIEWPORT ScreenViewPort;

	IDXGISwapChain* pSwapChain;
	UINT M4MassQuality;

	bool isEnable4xMsaa;
	ID3D11BlendState* alphaDisableState;
	ID3D11BlendState* alphaState;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilState* depthStencilDisableState;
public:
	HRESULT CreateDevice(void);
	HRESULT CreateSwapChain(void);
	void CreateDepthStencilState();
	void CreateBlendState();
	void OnResize(void);
	void Release(void);

public:
	void BeginDraw();
	void EndDraw();
	void ChangeZBuffer(bool isBuffer);
	void TurnOffAlphaBlend();
	void TurnOnAlphaBlend();
public:
	ID3D11Device* GetDevice() { return pD3dDevice; }
	ID3D11DeviceContext* GetContext() { return pD3dContext; }
	IDXGISwapChain* GetSwapChain() { return pSwapChain; }
	ID3D11DepthStencilView* GetDepthStencilView()const { return this->pDepthStencilView; }
	ID3D11RenderTargetView* GetRenderTargetView()const { return this->pRenderTargetView; }
	D3D_FEATURE_LEVEL GetFeatureLevel()const { return this->mFeatureLevel; }
	//TODO ResizeScreen func
};

#define pRenderer DxRenderer::Get()
#define Device DxRenderer::Get()->GetDevice()
#define DeviceContext DxRenderer::Get()->GetContext()
#define SwapChain DxRenderer::Get()->GetSwapChain()