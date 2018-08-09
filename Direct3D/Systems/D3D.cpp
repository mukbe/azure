#include "stdafx.h"
#include "D3D.h"


/*
<초기화의 순서>
D3D11CreateDevice 함수를 이용하여 ID3D11Device 인터페이스와
ID3D11DeviceContext 인터페이스를 생성한다.

ID3D11Device::CheckMultisampleQualityLevels 메서드를 이용해
4XMSAA 품질 수준 지원 여부를 확인

DXGI_SWAP_CHAIN_DESC 구조체 생성 (pSwapChain)

장치를 생성하는데 사용했던 IDXGIFactory 인터페이스를 이용하여
IDXGISwapChain 인스턴스 생성

SwapChain 의 backBuffer 에 대한 RenderView 생성

DepthStancilBuffer 와 연결되는 DepthStencilView 생성

RenderTargetView 와 DepthStencilView 를 Direct3D가 사용할 수 있도록
RenderPipeline의 출력의 병합 단계에 묶는다

Viewport 를 생성한다
*/


D3DDesc D3D::desc;

D3D::D3D()
	:mDriverType(D3D_DRIVER_TYPE_NULL),
	mFeatureLevel(D3D_FEATURE_LEVEL_11_0),
	isEnable4xMsaa(false),		//안티앨리어싱
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

	//	Direct3D 11 부터 기능 수준(feature level) 이라는 개념이 도입
	//	D3D_FEATURE_LEVEL 이라는 열거형(enum)으로 대표된 기능 수준
	//	DX9 ~ DX11 까지 여러 Direct3D 버전들에 대응할 수 있다

	//	사용자의 하드웨어가 특정 기능 수준을 지원하지 않는다면
	//	그보다 더 낮은(오래된) 기능 수준으로 내려가 응용 프로그램을 실행한다
	//	아래 코드는 검사할 때 최신 기능 수준부터 차례로 검사하여 최신 버전을
	//	기준으로 내려간다
	D3D_FEATURE_LEVEL FeatureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3
	};

	UINT nDriverType = ARRAYSIZE(DriverTypes);
	UINT nFeatureLevels = ARRAYSIZE(FeatureLevels);

	//	디바이스 생성
	for (UINT driverTypeIndex = 0; driverTypeIndex < nDriverType; driverTypeIndex++) {
		mDriverType = DriverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(
			nullptr,			//	pAdapter : 디스플레이 어댑터 지정 (null 이면 기본 디스플레이 어댑터 사용)
			mDriverType,		//	DriverType : 렌더링 3차원 그래픽 가속이 적용되게 하기 위해
								//	매개변수에 D3D_DRIVER_TYPE_HARDWARE 를 지정하는것이 좋음
								//	D3D_DRIVER_TYPE_REFERENCE : 표준 장치 생성 (정확성 목표, 극도로 느림)
								//	-.	하드웨어가 지원하지 않는 기능 시험 (ex 그래픽 카드가 없는 컴퓨터)
								//	-.	구동기의 버그를 시험
								//	D3D_DRIVER_TYPE_SOFTWARE : CPU로 돌린다. GPU 를 흉내낸다
								//	D3D_DRIVER_TYPE_WARP	 : 고성능 Direct3D 10.1 소프트웨어 구동기 설정
								//	11에서는 지원 안해줌
			nullptr,	//	Software : 소프트웨어 구동기 지정 (null : 하드웨어)
			DeviceFlags,	//	Flags : 추가 장치 생성 플래그 (or 로 결합 가능)
							//	D3D11_CREATE_DEVICE_DEBUG : 디버그 모드 빌드, 계층 활성화
							//	D3D11_CREATE_DEVICE_SINGLETHREADED : D3D 가 한 개의 스레드에서만 호출
			FeatureLevels,	//	pFeatureLevels : D3D_FEATURE_LEVEL 원소 배열
			nFeatureLevels,	//	FeatureLevels : D3D_FEATURE_LEVEL 원소 개수
			D3D11_SDK_VERSION,	//	SDK 버전
			&pD3dDevice,		//	반환될 장치
			&mFeatureLevel,		//	장치 기능 레벨 반환
			&pD3dContext);		//	장치 반환

								//	DirectX 11.0 플랫폼은 D3D_FEATURE_LEVEL_11_1 을 인식하지 못하므로 다시 시도해야 한다
		if (hr == E_INVALIDARG) {
			hr = D3D11CreateDevice(nullptr, mDriverType, nullptr, DeviceFlags, &FeatureLevels[1], nFeatureLevels - 1,
				D3D11_SDK_VERSION, &pD3dDevice, &mFeatureLevel, &pD3dContext);
		}

		if (SUCCEEDED(hr)) break;
	}

	if (FAILED(hr)) {
		MessageBox(0, L"디바이스 초기화 실패!", 0, MB_OK);
		return hr;
	}

	return hr;
}

HRESULT D3D::CreateSwapChain(void)
{
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC sd;

	sd.BufferDesc.Width = desc.Width;				//	후면 버퍼 너비
	sd.BufferDesc.Height = desc.Height;				//	후면 버퍼 높이
	sd.BufferDesc.RefreshRate.Numerator = 60;			//	디스플레이 모드 최대 갱신율 (FPS)
	sd.BufferDesc.RefreshRate.Denominator = 1;			//	최소
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//	후면 버퍼 픽셀 형식
														//	DXGI_FORMAT_R8G8B8A8_UNORM 형식(8비트 적, 녹, 청, 알파) 를 사용하는 이유는
														//	일반적인 모니터의 색상당 비트 수가 24비트 이하이므로 정밀도를 높게 잡아봤자
														//	낭비일 뿐. 32 비트 이상이면 바꿔줄 필요 있음
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	//	디스플레이 스캔라인 모드
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	//	디스플레이 비례 모드
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		//	버퍼 용도
	sd.BufferCount = 1;							//	버퍼 개수 (1 : 이중 버퍼링)
	sd.OutputWindow = desc.Handle;			//	렌더링 결과 표시 창
	sd.Windowed = true;			//	창모드 true, 전체화면 false
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	//	스왑 효과. 알아서 효율적인걸로 잡아줌
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;		//	추가 플래그
															//	DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 전체화면 모드로 전환할 때 현 후면 버퍼
															//	설정에 가장 잘 부합하는 디스플레이 모드로 자동 변경
															//	그냥 0 해서 무시해도 된다. 이유?
															//	요즘엔 기본적으로 전체화면 모드에서 데스크톱 디스플레이 모드를 사용해도 괜찮

															//	해당 컴퓨터가 4x Msaa 를 지원하는가?
															//	Direct11 의 모든 렌더링을 4x Msaa 로 그릴 수 있는지 확인
	hr = pD3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		4,
		&M4MassQuality);

	//	multisample anti-aliasing
	//	다중표본화를 이용한 앨리어싱을 제거하는가?
	if (isEnable4xMsaa) {
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = M4MassQuality - 1;
	}
	else {
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	/*	스왑 체인을 올바르게 만들려면 dxgi 를 사용해야하며
	디바이스 장치를 만드는데 사용되며, 다른 dxgi 를 사용하려고 하면
	CreateDXGIFactory 를 호출하여 오류가 발생된다

	이 함수는 다른 IDXGIFactory 의 장치로 호출된다.
	*/

	IDXGIDevice* dxgiDevice = nullptr;
	pD3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

	IDXGIAdapter* dxgiAdapter = nullptr;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	//	IDXGIFactory 인터페이스 휙득
	IDXGIFactory* dxgiFactory = nullptr;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	//	SwapChain 생성
	dxgiFactory->CreateSwapChain(pD3dDevice, &sd, &pSwapChain);

	//	필요없는 인터페이스 해제
	SafeRelease(dxgiDevice);
	SafeRelease(dxgiAdapter);
	SafeRelease(dxgiFactory);

	//	참고로 DXGI(DirectX Graphics Infrastructure) 는 Direct3D 와는 개별적인 API로,
	//	스왑체인이나 그래픽 하드웨어, 창 몯와 전체화면 모드 전환 등 그래픽에 관련된
	//	작업을 처리한다.
	//	Direct3D 와 분리한 이유는 다른 그래픽 API(ex D2D)에도 교환 사슬 등등을 사용하기
	//	때문. 덕분에 여러 그래픽 API 들을 공통의 DXGI API 를 사용해서 작업 처리가 가능!

	OnResize();
	CreateDepthStencilState();
	CreateBlendState();
	return hr;
}

void D3D::CreateDepthStencilState()
{
	//스탠실 상태 초기화 
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };
	depthStencilDesc.DepthEnable = true;									//깊이버퍼 활성화 여부
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;			//깊이버퍼 쓰기 활성화
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;						//깊이 테이터 와 기존 데이터 비교(원본 데이터가 대상 데이터보다 작으면 통과) 

	depthStencilDesc.StencilEnable = true;									//표면 버퍼 여부
	depthStencilDesc.StencilReadMask = 0xFF;								//읽는데 필요한 일부 식별
	depthStencilDesc.StencilWriteMask = 0xFF;								//쓰는데 필요한 일부 식별
																			//표면 법선이 카메라를 향하고 있을때 각버퍼의 테스트 결과 
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		//표면 버퍼 실패시	 (기존 스탠실 유지)
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;	//표면 버퍼 성공 , 깊이 버퍼 실패시 (표면 값 1증가)
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//모두 통과
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//현 표면버퍼를 기존 버퍼와 비교 (항상 비교)
																			//표면 법선이 카메라와멀어 젔을때
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HRESULT hr = _Device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	assert(SUCCEEDED(hr));

	//스텐실 사용 안함 
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

	//	assert 는 에러나면 창 띄우지만 전처리기로 만든 Assert 는 디버그창에 출력해준다
	assert(SUCCEEDED(pD3dContext));
	assert(SUCCEEDED(pD3dDevice));
	assert(SUCCEEDED(pSwapChain));

	//	버퍼에 대한 참조 값이 남아있으므로
	//	남아있던 뷰를 모두 해제하고 파괴한다

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

	//	뎁스스탠실 버퍼와 뷰 생성
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = desc.Width;		//	너비
	depthStencilDesc.Height =desc.Height;	//	높이
	depthStencilDesc.MipLevels = 1;				//	밉맵 수준의 개수
	depthStencilDesc.ArraySize = 1;				//	배열의 텍스처 개수
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//	텍셀 형식

	if (isEnable4xMsaa) {
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = M4MassQuality - 1;
	}
	else {
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;	//	텍스처의 용도
													//	D3D11_USAGE_DEFAULT : GPU만 읽고 쓸 수 있도록 한다 (CPU X)
													//	뎁스스텐실 버퍼를 위한 텍스처를 생성할 때는 요걸로 한다
													//	D3D11_USAGE_IMMUTABLE : 자원을 생성하고 변경하지 않을경우 사용
													//	GPU 읽기 전용이 되어 몇 가지 최적화가 가능해진다.
													//	D3D11_USAGE_DYNAMIC : 응용 프로그램(CPU)이 자원의 내용을 빈번하게 갱신해야 할 경우
													//	GPU 읽기, CPU 쓰기 가능. 허나 성능상에 좋지 않다.
													//	D3D11_USAGE_STAGING : CPU가 복사본을 읽어야 한다면 사용. 느림!
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	//	파이프라인에 어떻게
															//	묶을 것인지 지정. OR 가능
															//	D3D11_BIND_RENDER_TARGET : 텍스쳐를 렌더 대상으로 파이프라인에 묶는다
															//	D3D11_BIND_SHADER_RESOURCES : 텍스쳐를 셰이더 자원으로 파이프라인에 묶는다
	depthStencilDesc.CPUAccessFlags = 0;	//	CPU 자원 방식 결정. 
											//	D3D11_CPU_ACCESS_WRITE : CPU 가 자료를 기록해야한다면 사용
											//	안하는게 좋음
	depthStencilDesc.MiscFlags = 0;	//	기타 플래그. 뎊스스텐실에는 지원 안해주므로 0

	pD3dDevice->CreateTexture2D(&depthStencilDesc, 0, &pDepthStencilBuffer);
	pD3dDevice->CreateDepthStencilView(pDepthStencilBuffer, 0, &pDepthStencilView);

	//	렌더타겟 뷰와 뎁스스텐실 뷰를 파이프라인에 바인딩
	pD3dContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);

	//	뷰포트 설정
	//	어디서부터 어디까지 그릴건지 지정해준다
	//	2인용 모드를 위해 화면을 분할할 수도 있다!
	//	아니면 UI 를 사용하기 위해 공간을 비워둘 수 있다
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
	// 혼합 요소를 설정합니다.
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// 알파 블렌딩을 끕니다.
	_Context->OMSetBlendState(alphaDisableState, blendFactor, 0xffffffff);
}

void D3D::TurnOnAlphaBlend()
{
	// 혼합 요소를 설정합니다.
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// 알파 블렌딩을 끕니다.
	_Context->OMSetBlendState(alphaState, blendFactor, 0xffffffff);
}
