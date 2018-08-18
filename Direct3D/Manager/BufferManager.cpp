#include "stdafx.h"
#include "BufferManager.h"

SingletonCpp(BufferManager)

BufferManager::BufferManager()
{
}


BufferManager::~BufferManager()
{
	ShaderBufferIter iter = bufferContainer.begin();

	for (; iter != bufferContainer.end(); ++iter)
	{
		SafeDelete(iter->second);
	}

	bufferContainer.clear();
}
