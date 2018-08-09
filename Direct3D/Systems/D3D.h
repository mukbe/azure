#pragma once
#include "DefineHeader.h"
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


class D3D
{
private:
	Singleton(D3D)
public:
	static void GetDesc(struct D3DDesc* desc) { *desc = D3D::desc; }
	static void SetDesc(struct D3DDesc desc) { D3D::desc = desc; }
public:
	static D3DDesc desc;

private:
	ID3D11Device * pD3dDevice;
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
	void EndShadowDraw();
	void BeginDraw();
	void EndDraw();
	void ChangeZBuffer(bool isBuffer);
	void TurnOffAlphaBlend();
	void TurnOnAlphaBlend();
public:
	ID3D11Device * GetDevices() { return pD3dDevice; }
	ID3D11DeviceContext* GetContext() { return pD3dContext; }
};

#define pRenderer D3D::GetInstance()
#define _Device D3D::GetInstance()->GetDevices()
#define _Context D3D::GetInstance()->GetContext()