#pragma once
#include "IndexCreator.h"

// 텍스처에 관한 모든 디스크립터를 모아둔 클래스

// Texture(SRV) 또는 UAV 리소스를 공유하고 유지하기 위해 Descriptor를 동적으로 할당하고 관리할 수 있는 체계.
// 셰이더 제출 디스크립터 힙(Des)에 복사할 Source 디스크립터 힙 용도
 
class SingleDescriptorAllocator
{
	ComPtr<ID3D12DescriptorHeap>	_SRVUAVDescHeap = nullptr;	// use for SRV, UAV
	ComPtr<ID3D12DescriptorHeap>	_RTVDescHeap = nullptr;		// use for RTV
	ComPtr<ID3D12DescriptorHeap>	_DSVDescHeap = nullptr;		// use for DSV

	UINT							_SRVUAVDescSize = 0;
	UINT							_RTVDescSize = 0;
	UINT							_DSVDescSize = 0;

	IndexCreator					_indexCreatorForSRVUAV;
	IndexCreator					_indexCreatorForRTV;
	IndexCreator					_indexCreatorForDSV;



	void Cleanup();

public:
	BOOL Init(DWORD srvMaxCount, DWORD rtvMaxCount, DWORD dsvMaxCount, D3D12_DESCRIPTOR_HEAP_FLAGS flags);

	BOOL	AllocDescriptorHandleForSRVUAV(D3D12_CPU_DESCRIPTOR_HANDLE* outDescriptorHandle);
	void	FreeDescriptorHandleForSRVUAV(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);
	BOOL	CheckForSRVUAV(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);

	BOOL	AllocDescriptorHandleForRTV(D3D12_CPU_DESCRIPTOR_HANDLE* outDescriptorHandle);
	void	FreeDescriptorHandleForRTV(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);
	BOOL	CheckForRTV(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);

	BOOL	AllocDescriptorHandleForDSV(D3D12_CPU_DESCRIPTOR_HANDLE* outDescriptorHandle);
	void	FreeDescriptorHandleForDSV(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);
	BOOL	CheckForDSV(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);

	D3D12_GPU_DESCRIPTOR_HANDLE	GetGPUHandleFromCPUHandleForSRVUAV(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE	GetGPUHandleFromCPUHandleForRTV(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE	GetGPUHandleFromCPUHandleForDSV(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);

	ComPtr<ID3D12DescriptorHeap> GetSRVUAVDescriptorHeap() { return _SRVUAVDescHeap; }
	ComPtr<ID3D12DescriptorHeap> GetRTVDescriptorHeap() { return _RTVDescHeap; }
	ComPtr<ID3D12DescriptorHeap> GetDSVDescriptorHeap() { return _DSVDescHeap; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVUAVDescriptorHeapStart() { return _SRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVDescriptorHeapStart() { return _RTVDescHeap->GetCPUDescriptorHandleForHeapStart(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVDescriptorHeapStart() { return _DSVDescHeap->GetCPUDescriptorHandleForHeapStart(); }

	UINT GetSRVUAVDescriptorSize() const { return _SRVUAVDescSize; }
	UINT GetRTVDescriptorSize() const { return _RTVDescSize; }
	UINT GetDSVDescriptorSize() const { return _DSVDescSize; }

	SingleDescriptorAllocator();
	~SingleDescriptorAllocator();
};

