#include "stdafx.h"
#include "Window.h"
#include "../Program.h"
#include "./Model/Model.h"

Program* Window::program = nullptr;

WPARAM Window::Run()
{
	MSG msg = { 0 };

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);

//=========Init Manager=================================
	DxRenderer::Create();
	pRenderer->CreateDevice();
	pRenderer->CreateSwapChain();

	Keyboard::Create();
	Mouse::Create();

	Time::Create();
	Time::Get()->Start();

	ResourceManager::Create();

	ShaderManager::Create();
	Shaders->Init();

	BufferManager::Create();
	BufferManager::Get()->Init();

	Gizmo::Create();
	RenderManager::Create();
	SceneManager::Create();
	ObjectManager::Create();
	FactoryManager::Create();
	DataBase::Create();
	CameraManager::Create();

	ImGui::Create(desc.Handle, Device, DeviceContext);
	ImGui::StyleColorsDark();
//=====================================================

	program = new Program();
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Time::Get()->Update();

			if (ImGui::IsMouseHoveringAnyWindow() == false)
			{
				Keyboard::Get()->Update();
				Mouse::Get()->Update();
			}

			program->PreUpdate();
			program->Update();
			program->PostUpdate();
			ImGui::Update();

			program->Render();			
		}
	}
	SafeDelete(program);


//================Release Manager===============================
	ImGui::Delete();

	CameraManager::Delete();
	DataBase::Delete();
	FactoryManager::Delete();
	ObjectManager::Get()->Release();
	ObjectManager::Delete();
	Scenes->Delete();
	RenderRequest->Delete();
	Gizmo::Delete();
	Buffers->Delete();
	Shaders->Delete();
	ResourceManager::Delete();
	Time::Delete();
	Mouse::Delete();
	Keyboard::Delete();

	pRenderer->Delete();
//=============================================================


	return msg.wParam;
}

Window::Window()
{
	D3DDesc desc;
	DxRenderer::GetDesc(&desc);

	WNDCLASSEX wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hIconSm = wndClass.hIcon;
	wndClass.hInstance = desc.Instance;
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.lpszClassName = desc.AppName.c_str();
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbSize = sizeof(WNDCLASSEX);

	WORD wHr = RegisterClassEx(&wndClass);
	assert(wHr != 0);

	if (desc.bFullScreen == true)
	{
		DEVMODE devMode = { 0 };
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmPelsWidth = (DWORD)desc.Width;
		devMode.dmPelsHeight = (DWORD)desc.Height;
		devMode.dmBitsPerPel = 32;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	}

	desc.Handle = CreateWindowEx
	(
		WS_EX_APPWINDOW
		, desc.AppName.c_str()
		, desc.AppName.c_str()
		, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, NULL
		, (HMENU)NULL
		, desc.Instance
		, NULL
	);
	assert(desc.Handle != NULL);
	DxRenderer::SetDesc(desc);


	RECT rect = { 0, 0, (LONG)desc.Width, (LONG)desc.Height };

	UINT centerX = (GetSystemMetrics(SM_CXSCREEN) - (UINT)desc.Width) / 2;
	UINT centerY = (GetSystemMetrics(SM_CYSCREEN) - (UINT)desc.Height) / 2;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	MoveWindow
	(
		desc.Handle
		, centerX, centerY
		, rect.right - rect.left, rect.bottom - rect.top
		, TRUE
	);
	ShowWindow(desc.Handle, SW_SHOWNORMAL);
	SetForegroundWindow(desc.Handle);
	SetFocus(desc.Handle);

	ShowCursor(true);
}

Window::~Window()
{
	D3DDesc desc;
	DxRenderer::GetDesc(&desc);

	if (desc.bFullScreen == true)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(desc.Handle);

	UnregisterClass(desc.AppName.c_str(), desc.Instance);
}

LRESULT CALLBACK Window::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	Mouse::Get()->InputProc(message, wParam, lParam);

	if (ImGui::WndProc((UINT*)handle, message, wParam, lParam))
		return true;

	if (message == WM_SIZE)
	{
		ImGui::Invalidate();
		{
			if (program != NULL)
			{
				float width = (float)LOWORD(lParam);
				float height = (float)HIWORD(lParam);

				if (pRenderer != NULL)
					//pRenderer->ResizeScreen(width, height);
				
				program->ResizeScreen();
			}		
		}
		ImGui::Validate();
	}

	if (message == WM_CLOSE || message == WM_DESTROY)
	{
		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

