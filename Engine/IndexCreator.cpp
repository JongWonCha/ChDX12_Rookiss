#include "pch.h"
#include "IndexCreator.h"

IndexCreator::IndexCreator()
{
}

BOOL IndexCreator::Initialize(DWORD count)
{
	_indexTable = new DWORD[count];
	memset(_indexTable, 0, sizeof(DWORD) * count);
	_maxNum = count;

	for (DWORD i = 0; i < count; ++i)
		_indexTable[i] = i;

	return TRUE;
}

DWORD IndexCreator::Alloc()
{
	DWORD result = -1;
	if (_allocatedCount >= _maxNum) return result;

	result = _indexTable[_allocatedCount];
	_allocatedCount++;
	
	return result;
}

void IndexCreator::Free(DWORD index)
{
	if (!_allocatedCount)
	{
		__debugbreak();
	}
	_allocatedCount--;
	_indexTable[_allocatedCount] = index;
}

void IndexCreator::Cleanup()
{
	if (_indexTable)
	{
		delete[] _indexTable;
		_indexTable = nullptr;
	}
}

void IndexCreator::Check()
{
	if (_allocatedCount)
		__debugbreak();
}

IndexCreator::~IndexCreator()
{
	Check();
	Cleanup();
}