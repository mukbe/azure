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

	class ComputeShader* CreateComputeShader(string key, wstring fileName, string entryPoint);
	ComputeShader* FindComputeShader(string key);
public:
	void Init();
	void ShaderRelease();

private:
	typedef map<string, Shader*> ShadersContain;
	typedef map<string, Shader*>::iterator ShadersIter;
	typedef map<string, ComputeShader*> ComputeShadersContain;
	typedef map<string, ComputeShader*>::iterator ComputeShadersIter;

private:
	ShadersContain shaders;
	Shader* currentShader;
	ComputeShadersContain computeShaders;
	class ComputeShader* computeShader;
};

#define Shaders ShaderManager::Get()