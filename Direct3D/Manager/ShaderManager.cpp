#include "stdafx.h"
#include "ShaderManager.h"



ShaderManager::ShaderManager()
{
}


ShaderManager::~ShaderManager()
{
}
Shader * ShaderManager::CreateShader(string key, wstring fileName, Shader::ShaderType type, string funcName)
{
	Shader* shader;
	shader = FindShader(key);

	if (shader == nullptr)
	{
		shader = new Shader(Shaders + fileName, type, funcName);
	}

	return shader;
}

Shader * ShaderManager::FindShader(string key)
{
	ShadersIter Iter = shaders.begin();

	for (Iter; Iter != shaders.end(); ++Iter)
	{
		if (key == Iter->first)
			return Iter->second;
	}

	return nullptr;
}

void ShaderManager::BindShader(string key)
{
	currentShader = FindShader(key);

	if(currentShader != nullptr)
		currentShader->Render();
}

void ShaderManager::ReleaseShader()
{
	currentShader->ReleaseShader();
}

void ShaderManager::Release()
{
	ShadersIter Iter = shaders.begin();

	for (Iter; Iter != shaders.end(); ++Iter)
	{
		Iter->second->ReleaseShader();
	}

}
