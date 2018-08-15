#pragma once

class ShaderManager
{
	Singleton(ShaderManager)

public:
	Shader * CreateShader(string key, wstring fileName, Shader::ShaderType type = Shader::ShaderType::Default, string funcName = "");
	Shader * FindShader(string key);
	void BindShader(string key);
	void ReleaseShader();
public:
	void Release();

private:
	typedef map<string, Shader*> ShadersContain;
	typedef map<string, Shader*>::iterator ShadersIter;

private:
	ShadersContain shaders;
	Shader* currentShader;
};

