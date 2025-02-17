#pragma once
#include "IndexCreator.h"

// �ؽ�ó�� ���� ��� ��ũ���͸� ��Ƶ� Ŭ����

// Texture(SRV) �Ǵ� UAV ���ҽ��� �����ϰ� �����ϱ� ���� Descriptor�� �������� �Ҵ��ϰ� ������ �� �ִ� ü��.
// ���̴� ���� ��ũ���� ��(Des)�� ������ Source ��ũ���� �� �뵵
 
class SingleDescriptorAllocator
{
	ComPtr<ID3D12DescriptorHeap>	_SRVUAVDescHeap = nullptr;	// use for SRV, UAV
	
	UINT							_SRVUAVDescSize = 0;

	IndexCreator					_indexCreator;

	void Cleanup();

public:
	BOOL	Init(DWORD maxCount, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
	BOOL	AllocDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* outDescriptorHandle);
	void	FreeDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);
	BOOL	Check(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE	GetGPUHandleFromCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);
	ComPtr<ID3D12DescriptorHeap> GetSRVUAVDescriptorHeap() { return _SRVUAVDescHeap; }

	SingleDescriptorAllocator();
	~SingleDescriptorAllocator();
};

