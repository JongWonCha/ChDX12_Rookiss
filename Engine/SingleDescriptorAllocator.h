#pragma once
#include "IndexCreator.h"


// Texture �Ǵ� UAV ���ҽ��� �����ϰ� �����ϱ� ���� Descriptor�� �������� �Ҵ��ϰ� ������ �� �ִ� ü��.
class SingleDescriptorAllocator
{
	ComPtr<ID3D12DescriptorHeap> _descHeap = nullptr;
	IndexCreator				_indexCreator;
	UINT						_descSize = 0;

	void Cleanup();

public:
	BOOL	Init(DWORD maxCount, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
	BOOL	AllocDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* outDescriptorHandle);
	void	FreeDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);
	BOOL	Check(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE	GetGPUHandleFromCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);
	ComPtr<ID3D12DescriptorHeap>	GetDescriptorHeap() { return _descHeap; }

	SingleDescriptorAllocator();
	~SingleDescriptorAllocator();
};

