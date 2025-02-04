#pragma once
class IndexCreator
{
	DWORD* _indexTable = nullptr;
	DWORD	_maxNum = 0;
	DWORD	_allocatedCount = 0;

public:
	BOOL Initialize(DWORD count);

	DWORD Alloc();
	void Free(DWORD index);
	void Cleanup();
	void Check();

	IndexCreator();
	~IndexCreator();
};

