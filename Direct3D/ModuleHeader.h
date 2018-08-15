#pragma once
//Manager
#include "./Systems/Keyboard.h"
#include "./Systems/Mouse.h"
#include "./Systems/Time.h"


#include "./Renders/VertexLayouts.h"
#include "./Renders/States.h"
#include "./Renders/ShaderBuffer.h"
#include "./Renders/Shader.h"

#include "./Utilities/Math.h"
#include "./Utilities/Json.h"
#include "./Utilities/String.h"
#include "./Utilities/Path.h"

#include "./Renders/Gizmo.h"
#include "./Manager/ShaderManager.h"

#define KeyCode Keyboard::Get()
#define GameMouse Mouse::Get()
#define DeltaTime Time::Delta()
#define Shaders ShaderManager::GetInstance()
