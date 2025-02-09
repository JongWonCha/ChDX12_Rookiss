#pragma once
#include "IndexCreator.h"


// Texture 또는 UAV 리소스를 공유하고 유지하기 위해 Descriptor를 동적으로 할당하고 관리할 수 있는 체계.
// 셰이더 제출 디스크립터 힙(Des)에 복사할 Source 디스크립터 힙 용도
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

