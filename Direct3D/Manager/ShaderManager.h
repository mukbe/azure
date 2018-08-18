#pragma once
#include "DefineHeader.h"
class ShaderManager
{
	 SingletonHeader(ShaderManager)
public:
	Shader * CreateShader(string key, wstring fileName, Shader::ShaderType type = Shader::ShaderType::Default, string funcName = "");
	Shader * FindShader(string key);
	void BindShader(string key);
	void ReleaseShader();
public:
	void Init();


	void ShaderRelease();
private:
	typedef map<string, Shader*> ShadersContain;
	typedef map<string, Shader*>::iterator ShadersIter;

private:
	ShadersContain shaders;
	Shader* currentShader;
};

#define Shaders ShaderManager::Get()