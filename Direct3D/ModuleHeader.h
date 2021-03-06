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
#include "./Utilities/DebugHelper.h"

#include "./Manager/BufferManager.h"
#include "./Manager/ShaderManager.h"
#include "./Manager/SceneManager.h"
#include "./Manager/RenderManager.h"
#include "./Manager/ObjectManager.h"
#include "./Manager/CameraManager.h"

#include "./Renders/Gizmo.h"
#include "./Manager/FactoryManager.h"
#include "./Manager/ResourceManager.h"
#include "./Manager/DataBase.h"

#define KeyCode Keyboard::Get()
#define GameMouse Mouse::Get()
#define DeltaTime Time::Delta()


#include "./ComputeShader/ComputeShader.h"
#include "./ComputeShader/ComputeResource.h"

#include "./Physics/Physics.h"
