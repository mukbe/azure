#include "stdafx.h"
#include "BufferManager.h"



BufferManager::BufferManager()
{
}


BufferManager::~BufferManager()
{
}

void BufferManager::Release()
{
	ShaderBufferIter iter = bufferContainer.begin();

	for (; iter != bufferContainer.end(); ++iter)
	{
		SafeDelete(iter->second);
	}

	bufferContainer.clear();
}