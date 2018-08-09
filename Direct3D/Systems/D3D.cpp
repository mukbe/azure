#include "stdafx.h"
#include "D3D.h"


/*
<�ʱ�ȭ�� ����>
D3D11CreateDevice �Լ��� �̿��Ͽ� ID3D11Device �������̽���
ID3D11DeviceContext �������̽��� �����Ѵ�.

ID3D11Device::CheckMultisampleQualityLevels �޼��带 �̿���
4XMSAA ǰ�� ���� ���� ���θ� Ȯ��

DXGI_SWAP_CHAIN_DESC ����ü ���� (pSwapChain)

��ġ�� �����ϴµ� ����ߴ� IDXGIFactory �������̽��� �̿��Ͽ�
IDXGISwapChain �ν��Ͻ� ����

SwapChain �� backBuffer �� ���� RenderView ����

DepthStancilBuffer �� ����Ǵ� DepthStencilView ����

RenderTargetView �� DepthStencilView �� Direct3D�� ����� �� �ֵ���
RenderPipeline�� ����� ���� �ܰ迡 ���´�

Viewport �� �����Ѵ�
*/


D3DDesc D3D::desc;

D3D::D3D()
	:mDriverType(D3D_DRIVER_TYPE_NULL),
	mFeatureLevel(D3D_FEATURE_LEVEL_11_0),
	isEnable4xMsaa(false),		//��Ƽ�ٸ����
	M4MassQuality(0),
	pSwapChain(nullptr),
	pDepthStencilBuffer(nullptr),
	pDepthStencilView(nullptr),
	pD3dContext(nullptr),
	pD3dDevice(nullptr),
	pRenderTargetView(nullptr)
{
	ZeroMemory(&ScreenViewPort, sizeof D3D11_VIEWPORT);
}

D3D::~D3D()
{
	Release();
}

HRESULT D3D::CreateDevice()
{
	HRESULT hr;

	UINT DeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE DriverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	//	Direct3D 11 ���� ��� ����(feature level) �̶�� ������ ����
	//	D3D_FEATURE_LEVEL �̶�� ������(enum)���� ��ǥ�� ��� ����
	//	DX9 ~ DX11 ���� ���� Direct3D �����鿡 ������ �� �ִ�

	//	������� �ϵ��� Ư�� ��� ������ �������� �ʴ´ٸ�
	//	�׺��� �� ����(������) ��� �������� ������ ���� ���α׷��� �����Ѵ�
	//	�Ʒ� �ڵ�� �˻��� �� �ֽ� ��� ���غ��� ���ʷ� �˻��Ͽ� �ֽ� ������
	//	�������� ��������
	D3D_FEATURE_LEVEL FeatureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3
	};

	UINT nDriverType = ARRAYSIZE(DriverTypes);
	UINT nFeatureLevels = ARRAYSIZE(FeatureLevels);

	//	����̽� ����
	for (UINT driverTypeIndex = 0; driverTypeIndex < nDriverType; driverTypeIndex++) {
		mDriverType = DriverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(
			nullptr,			//	pAdapter : ���÷��� ����� ���� (null �̸� �⺻ ���÷��� ����� ���)
			mDriverType,		//	DriverType : ������ 3���� �׷��� ������ ����ǰ� �ϱ� ����
								//	�Ű������� D3D_DRIVER_TYPE_HARDWARE �� �����ϴ°��� ����
								//	D3D_DRIVER_TYPE_REFERENCE : ǥ�� ��ġ ���� (��Ȯ�� ��ǥ, �ص��� ����)
								//	-.	�ϵ��� �������� �ʴ� ��� ���� (ex �׷��� ī�尡 ���� ��ǻ��)
								//	-.	�������� ���׸� ����
								//	D3D_DRIVER_TYPE_SOFTWARE : CPU�� ������. GPU �� �䳻����
								//	D3D_DRIVER_TYPE_WARP	 : ���� Direct3D 10.1 ����Ʈ���� ������ ����
								//	11������ ���� ������
			nullptr,	//	Software : ����Ʈ���� ������ ���� (null : �ϵ����)
			DeviceFlags,	//	Flags : �߰� ��ġ ���� �÷��� (or �� ���� ����)
							//	D3D11_CREATE_DEVICE_DEBUG : ����� ��� ����, ���� Ȱ��ȭ
							//	D3D11_CREATE_DEVICE_SINGLETHREADED : D3D �� �� ���� �����忡���� ȣ��
			FeatureLevels,	//	pFeatureLevels : D3D_FEATURE_LEVEL ���� �迭
			nFeatureLevels,	//	FeatureLevels : D3D_FEATURE_LEVEL ���� ����
			D3D11_SDK_VERSION,	//	SDK ����
			&pD3dDevice,		//	��ȯ�� ��ġ
			&mFeatureLevel,		//	��ġ ��� ���� ��ȯ
			&pD3dContext);		//	��ġ ��ȯ

								//	DirectX 11.0 �÷����� D3D_FEATURE_LEVEL_11_1 �� �ν����� ���ϹǷ� �ٽ� �õ��ؾ� �Ѵ�
		if (hr == E_INVALIDARG) {
			hr = D3D11CreateDevice(nullptr, mDriverType, nullptr, DeviceFlags, &FeatureLevels[1], nFeatureLevels - 1,
				D3D11_SDK_VERSION, &pD3dDevice, &mFeatureLevel, &pD3dContext);
		}

		if (SUCCEEDED(hr)) break;
	}

	if (FAILED(hr)) {
		MessageBox(0, L"����̽� �ʱ�ȭ ����!", 0, MB_OK);
		return hr;
	}

	return hr;
}

HRESULT D3D::CreateSwapChain(void)
{
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC sd;

	sd.BufferDesc.Width = desc.Width;				//	�ĸ� ���� �ʺ�
	sd.BufferDesc.Height = desc.Height;				//	�ĸ� ���� ����
	sd.BufferDesc.RefreshRate.Numerator = 60;			//	���÷��� ��� �ִ� ������ (FPS)
	sd.BufferDesc.RefreshRate.Denominator = 1;			//	�ּ�
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//	�ĸ� ���� �ȼ� ����
														//	DXGI_FORMAT_R8G8B8A8_UNORM ����(8��Ʈ ��, ��, û, ����) �� ����ϴ� ������
														//	�Ϲ����� ������� ����� ��Ʈ ���� 24��Ʈ �����̹Ƿ� ���е��� ���� ��ƺ���
														//	������ ��. 32 ��Ʈ �̻��̸� �ٲ��� �ʿ� ����
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	//	���÷��� ��ĵ���� ���
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	//	���÷��� ��� ���
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		//	���� �뵵
	sd.BufferCount = 1;							//	���� ���� (1 : ���� ���۸�)
	sd.OutputWindow = desc.Handle;			//	������ ��� ǥ�� â
	sd.Windowed = true;			//	â��� true, ��üȭ�� false
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	//	���� ȿ��. �˾Ƽ� ȿ�����ΰɷ� �����
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;		//	�߰� �÷���
															//	DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : ��üȭ�� ���� ��ȯ�� �� �� �ĸ� ����
															//	������ ���� �� �����ϴ� ���÷��� ���� �ڵ� ����
															//	�׳� 0 �ؼ� �����ص� �ȴ�. ����?
															//	���� �⺻������ ��üȭ�� ��忡�� ����ũ�� ���÷��� ��带 ����ص� ����

															//	�ش� ��ǻ�Ͱ� 4x Msaa �� �����ϴ°�?
															//	Direct11 �� ��� �������� 4x Msaa �� �׸� �� �ִ��� Ȯ��
	hr = pD3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		4,
		&M4MassQuality);

	//	multisample anti-aliasing
	//	����ǥ��ȭ�� �̿��� �ٸ������ �����ϴ°�?
	if (isEnable4xMsaa) {
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = M4MassQuality - 1;
	}
	else {
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	/*	���� ü���� �ùٸ��� ������� dxgi �� ����ؾ��ϸ�
	����̽� ��ġ�� ����µ� ���Ǹ�, �ٸ� dxgi �� ����Ϸ��� �ϸ�
	CreateDXGIFactory �� ȣ���Ͽ� ������ �߻��ȴ�

	�� �Լ��� �ٸ� IDXGIFactory �� ��ġ�� ȣ��ȴ�.
	*/

	IDXGIDevice* dxgiDevice = nullptr;
	pD3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

	IDXGIAdapter* dxgiAdapter = nullptr;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	//	IDXGIFactory �������̽� �׵�
	IDXGIFactory* dxgiFactory = nullptr;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	//	SwapChain ����
	dxgiFactory->CreateSwapChain(pD3dDevice, &sd, &pSwapChain);

	//	�ʿ���� �������̽� ����
	SafeRelease(dxgiDevice);
	SafeRelease(dxgiAdapter);
	SafeRelease(dxgiFactory);

	//	����� DXGI(DirectX Graphics Infrastructure) �� Direct3D �ʹ� �������� API��,
	//	����ü���̳� �׷��� �ϵ����, â ���� ��üȭ�� ��� ��ȯ �� �׷��ȿ� ���õ�
	//	�۾��� ó���Ѵ�.
	//	Direct3D �� �и��� ������ �ٸ� �׷��� API(ex D2D)���� ��ȯ �罽 ����� ����ϱ�
	//	����. ���п� ���� �׷��� API ���� ������ DXGI API �� ����ؼ� �۾� ó���� ����!

	OnResize();
	CreateDepthStencilState();
	CreateBlendState();
	return hr;
}

void D3D::CreateDepthStencilState()
{
	//���Ľ� ���� �ʱ�ȭ 
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };
	depthStencilDesc.DepthEnable = true;									//���̹��� Ȱ��ȭ ����
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;			//���̹��� ���� Ȱ��ȭ
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;						//���� ������ �� ���� ������ ��(���� �����Ͱ� ��� �����ͺ��� ������ ���) 

	depthStencilDesc.StencilEnable = true;									//ǥ�� ���� ����
	depthStencilDesc.StencilReadMask = 0xFF;								//�дµ� �ʿ��� �Ϻ� �ĺ�
	depthStencilDesc.StencilWriteMask = 0xFF;								//���µ� �ʿ��� �Ϻ� �ĺ�
																			//ǥ�� ������ ī�޶� ���ϰ� ������ �������� �׽�Ʈ ��� 
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		//ǥ�� ���� ���н�	 (���� ���Ľ� ����)
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;	//ǥ�� ���� ���� , ���� ���� ���н� (ǥ�� �� 1����)
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//��� ���
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//�� ǥ����۸� ���� ���ۿ� �� (�׻� ��)
																			//ǥ�� ������ ī�޶�͸־� ������
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HRESULT hr = _Device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	assert(SUCCEEDED(hr));

	//���ٽ� ��� ���� 
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc = { 0 };
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = _Device->CreateDepthStencilState(&depthDisabledStencilDesc, &depthStencilDisableState);
	assert(SUCCEEDED(hr));
}

void D3D::CreateBlendState()
{
	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	HRESULT hr = _Device->CreateBlendState(&blendStateDescription, &alphaDisableState);
	assert(SUCCEEDED(hr));

	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	hr = _Device->CreateBlendState(&blendStateDescription, &alphaState);
	assert(SUCCEEDED(hr));
}



void D3D::OnResize()
{
	if (pSwapChain == nullptr) return;

	//	assert �� �������� â ������� ��ó����� ���� Assert �� �����â�� ������ش�
	assert(SUCCEEDED(pD3dContext));
	assert(SUCCEEDED(pD3dDevice));
	assert(SUCCEEDED(pSwapChain));

	//	���ۿ� ���� ���� ���� ���������Ƿ�
	//	�����ִ� �並 ��� �����ϰ� �ı��Ѵ�

	SafeRelease(pRenderTargetView);
	SafeRelease(pDepthStencilView);
	SafeRelease(pDepthStencilBuffer);

	pSwapChain->ResizeBuffers(1,
		desc.Width,
		desc.Height,
		DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	ID3D11Texture2D* buffer;

	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&buffer));
	pD3dDevice->CreateRenderTargetView(buffer, 0, &pRenderTargetView);

	SafeRelease(buffer);

	//	�������Ľ� ���ۿ� �� ����
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = desc.Width;		//	�ʺ�
	depthStencilDesc.Height =desc.Height;	//	����
	depthStencilDesc.MipLevels = 1;				//	�Ӹ� ������ ����
	depthStencilDesc.ArraySize = 1;				//	�迭�� �ؽ�ó ����
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//	�ؼ� ����

	if (isEnable4xMsaa) {
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = M4MassQuality - 1;
	}
	else {
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;	//	�ؽ�ó�� �뵵
													//	D3D11_USAGE_DEFAULT : GPU�� �а� �� �� �ֵ��� �Ѵ� (CPU X)
													//	�������ٽ� ���۸� ���� �ؽ�ó�� ������ ���� ��ɷ� �Ѵ�
													//	D3D11_USAGE_IMMUTABLE : �ڿ��� �����ϰ� �������� ������� ���
													//	GPU �б� ������ �Ǿ� �� ���� ����ȭ�� ����������.
													//	D3D11_USAGE_DYNAMIC : ���� ���α׷�(CPU)�� �ڿ��� ������ ����ϰ� �����ؾ� �� ���
													//	GPU �б�, CPU ���� ����. �㳪 ���ɻ� ���� �ʴ�.
													//	D3D11_USAGE_STAGING : CPU�� ���纻�� �о�� �Ѵٸ� ���. ����!
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	//	���������ο� ���
															//	���� ������ ����. OR ����
															//	D3D11_BIND_RENDER_TARGET : �ؽ��ĸ� ���� ������� ���������ο� ���´�
															//	D3D11_BIND_SHADER_RESOURCES : �ؽ��ĸ� ���̴� �ڿ����� ���������ο� ���´�
	depthStencilDesc.CPUAccessFlags = 0;	//	CPU �ڿ� ��� ����. 
											//	D3D11_CPU_ACCESS_WRITE : CPU �� �ڷḦ ����ؾ��Ѵٸ� ���
											//	���ϴ°� ����
	depthStencilDesc.MiscFlags = 0;	//	��Ÿ �÷���. �X�����ٽǿ��� ���� �����ֹǷ� 0

	pD3dDevice->CreateTexture2D(&depthStencilDesc, 0, &pDepthStencilBuffer);
	pD3dDevice->CreateDepthStencilView(pDepthStencilBuffer, 0, &pDepthStencilView);

	//	����Ÿ�� ��� �������ٽ� �並 ���������ο� ���ε�
	pD3dContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);

	//	����Ʈ ����
	//	��𼭺��� ������ �׸����� �������ش�
	//	2�ο� ��带 ���� ȭ���� ������ ���� �ִ�!
	//	�ƴϸ� UI �� ����ϱ� ���� ������ ����� �� �ִ�
	ScreenViewPort.TopLeftX = 0;
	ScreenViewPort.TopLeftY = 0;
	ScreenViewPort.Width = static_cast<FLOAT>(desc.Width);
	ScreenViewPort.Height = static_cast<FLOAT>(desc.Height);
	ScreenViewPort.MinDepth = 0.0f;
	ScreenViewPort.MaxDepth = 1.0f;

	pD3dContext->RSSetViewports(1, &ScreenViewPort);
}

void D3D::Release()
{
	SafeRelease(alphaState);
	SafeRelease(alphaDisableState);

	SafeRelease(depthStencilDisableState);
	SafeRelease(depthStencilState);

	SafeRelease(pRenderTargetView);
	SafeRelease(pDepthStencilView);
	SafeRelease(pSwapChain);
	SafeRelease(pDepthStencilBuffer);

	if (pD3dContext)
		pD3dContext->ClearState();

	SafeRelease(pD3dContext);
	SafeRelease(pD3dDevice);
}

void D3D::EndShadowDraw()
{
	pD3dContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	pD3dContext->RSSetViewports(1, &ScreenViewPort);
	pD3dContext->RSSetState(NULL);
}

void D3D::BeginDraw()
{
	pD3dContext->ClearRenderTargetView(pRenderTargetView,
		D3DXCOLOR(0.49f, 0.49f, 0.49f, 1.0f));

	pD3dContext->ClearDepthStencilView(pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D::EndDraw()
{
	pSwapChain->Present(0, 0);
}

void D3D::ChangeZBuffer(bool isBuffer)
{
	if (isBuffer)
		pD3dContext->OMSetDepthStencilState(depthStencilState, 1);
	else
		pD3dContext->OMSetDepthStencilState(depthStencilDisableState, 1);
}

void D3D::TurnOffAlphaBlend()
{
	// ȥ�� ��Ҹ� �����մϴ�.
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// ���� ������ ���ϴ�.
	_Context->OMSetBlendState(alphaDisableState, blendFactor, 0xffffffff);
}

void D3D::TurnOnAlphaBlend()
{
	// ȥ�� ��Ҹ� �����մϴ�.
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// ���� ������ ���ϴ�.
	_Context->OMSetBlendState(alphaState, blendFactor, 0xffffffff);
}
