#pragma once
#include "../Renders/ShaderBuffer.h"
#include <map>

class BufferManager
{
	SingletonHeader(BufferManager)
private:
	typedef map<string, class ShaderBuffer*> ShaderBufferContainer;
	typedef map<string, class ShaderBuffer*>::iterator ShaderBufferIter;
private:
	ShaderBufferContainer bufferContainer;
public:
	template<class T> T* FindShaderBuffer();
	template<class T> void AddShaderBuffer(class ShaderBuffer* shaderBuffer);

	void Init();
};

template<class T> T* BufferManager::FindShaderBuffer()
{
	for (pair<string, class ShaderBuffer*> p : bufferContainer)
		if (T::GetCode() == p.first)return reinterpret_cast<T*>(p.second);

	T* t  = new T;
	this->bufferContainer.insert(make_pair(T::GetCode(), t));

	return t;
}

template<class T> void BufferManager::AddShaderBuffer(class ShaderBuffer* shaderBuffer)
{
	this->bufferContainer.insert(make_pair(T::GetCode(), shaderBuffer));
}

#define Buffers BufferManager::Get()

