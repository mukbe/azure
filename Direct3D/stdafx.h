#pragma once

#include <windows.h>
#include <assert.h>

//STL
#include <string>
#include <vector>
#include <list>
#include <map>
#include <functional>
using namespace std;


//Direct3D
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

//ImGui
#include <imgui.h>
#include <imguiDx11.h>
#pragma comment(lib, "imgui.lib")

//Fbx SDK
#define FBXSDK_SHARED
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")
using namespace fbxsdk;

//Json Cpp
#include <json/json.h>
#pragma comment(lib, "jsoncpp.lib")
//#pragma warning(disable:4996)


#include "./Systems/D3D.h"
#include "DefineHeader.h"
#include "ModuleHeader.h"


const wstring Assets = L"../_Assets/";
const wstring Contents = L"../_Contents/";
const wstring Textures = L"../_Contents/Textures/";
const wstring BinModels = L"../_Contents/BinModels/";
const wstring FbxModels = L"../_Contents/FbxModels/";
const wstring Shaders = L"../_Shaders/";


namespace Landscape
{
	typedef VertexTextureNormal TerrainVertexType;
}

namespace Water
{
	typedef VertexColorNormal WaveVertexType;
	typedef VertexColorNormal PondVertexType;
	typedef VertexTexture OceanVertexType;
}