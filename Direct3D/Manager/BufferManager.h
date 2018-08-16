#pragma once
#include "../Renders/ShaderBuffer.h"
#include <map>
class BufferManager
{
private:
	typedef map<string, class ShaderBuffer*> ShaderBufferContainer;
	typedef map<string, class ShaderBuffer*>::iterator ShaderBufferIter;
private:
	ShaderBufferContainer bufferContainer;
public:
	BufferManager();
	~BufferManager();

	void Release();

	template<class T> T* FindShaderBuffer();
	template<class T> void AddShaderBuffer(class ShaderBuffer* shaderBuffer);
};

template<class T> T* BufferManager::FindShaderBuffer()
{
	for (pair<string, class ShaderBuffer*> p : shaderBufferList)
		if (T::GetCode() == p.first)return reinterpret_cast<T*>(p.second);

	return new T;

}

template<class T> void BufferManager::AddShaderBuffer(class ShaderBuffer* shaderBuffer)
{
	this->shaderBufferList.insert(make_pair(T::GetCode(), shaderBuffer));
}
