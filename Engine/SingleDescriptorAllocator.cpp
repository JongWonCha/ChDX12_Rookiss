#include "pch.h"
#include "Engine.h"
#include "IndexCreator.h"
#include "SingleDescriptorAllocator.h"

SingleDescriptorAllocator::SingleDescriptorAllocator()
{
}

BOOL SingleDescriptorAllocator::Init(DWORD srvMaxCount, DWORD rtvMaxCount, DWORD dsvMaxCount, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{

	// Create SRV/UAV descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = srvMaxCount;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = flags;

	if (FAILED(DEVICE->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_SRVUAVDescHeap))))
	{
		__debugbreak();
		return FALSE;
	}

	_indexCreatorForSRVUAV.Initialize(srvMaxCount);
	_SRVUAVDescSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Create RTV descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = rtvMaxCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = flags;
	rtvHeapDesc.NodeMask = 0;

	if (FAILED(DEVICE->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_RTVDescHeap))))
	{
		__debugbreak();
		return FALSE;
	}

	_indexCreatorForRTV.Initialize(rtvMaxCount);
	_RTVDescSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Create DSV descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = dsvMaxCount;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = flags;

	if (FAILED(DEVICE->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&_DSVDescHeap))))
	{
		__debugbreak();
		return FALSE;
	}

	_indexCreatorForDSV.Initialize(dsvMaxCount);
	_DSVDescSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	return TRUE;
}

BOOL SingleDescriptorAllocator::AllocDescriptorHandleForSRVUAV(D3D12_CPU_DESCRIPTOR_HANDLE* outDescriptorHandle)
{
	DWORD dwIndex = _indexCreatorForSRVUAV.Alloc();
	if (dwIndex != -1)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(_SRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart(), dwIndex, _SRVUAVDescSize);
		*outDescriptorHandle = descriptorHandle;
		return TRUE;
	}
	return FALSE;
}

BOOL SingleDescriptorAllocator::CheckForSRVUAV(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE base = _SRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
	if (base.ptr > descriptorHandle.ptr)
	{
		__debugbreak();
		return FALSE;
	}

	return TRUE;
}

BOOL SingleDescriptorAllocator::AllocDescriptorHandleForRTV(D3D12_CPU_DESCRIPTOR_HANDLE* outDescriptorHandle)
{
	DWORD dwIndex = _indexCreatorForRTV.Alloc();
	if (dwIndex != -1)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(_RTVDescHeap->GetCPUDescriptorHandleForHeapStart(), dwIndex, _RTVDescSize);
		*outDescriptorHandle = descriptorHandle;
		return TRUE;
	}
	return FALSE;
}

void SingleDescriptorAllocator::FreeDescriptorHandleForRTV(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE base = _RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
#ifdef _DEBUG
	if (base.ptr > descriptorHandle.ptr)
	{
		__debugbreak();
		return;
	}
#endif
	DWORD dwIndex = (DWORD)(descriptorHandle.ptr - base.ptr) / _RTVDescSize;
	_indexCreatorForRTV.Free(dwIndex);
}

BOOL SingleDescriptorAllocator::CheckForRTV(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE base = _RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
	if (base.ptr > descriptorHandle.ptr)
	{
		__debugbreak();
		return FALSE;
	}

	return TRUE;
}

BOOL SingleDescriptorAllocator::AllocDescriptorHandleForDSV(D3D12_CPU_DESCRIPTOR_HANDLE* outDescriptorHandle)
{
	DWORD dwIndex = _indexCreatorForDSV.Alloc();
	if (dwIndex != -1)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(_DSVDescHeap->GetCPUDescriptorHandleForHeapStart(), dwIndex, _DSVDescSize);
		*outDescriptorHandle = descriptorHandle;
		return TRUE;
	}
	return FALSE;
}

void SingleDescriptorAllocator::FreeDescriptorHandleForDSV(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE base = _DSVDescHeap->GetCPUDescriptorHandleForHeapStart();
#ifdef _DEBUG
	if (base.ptr > descriptorHandle.ptr)
	{
		__debugbreak();
		return;
	}
#endif
	DWORD dwIndex = (DWORD)(descriptorHandle.ptr - base.ptr) / _DSVDescSize;
	_indexCreatorForDSV.Free(dwIndex);
}

BOOL SingleDescriptorAllocator::CheckForDSV(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE base = _DSVDescHeap->GetCPUDescriptorHandleForHeapStart();
	if (base.ptr > descriptorHandle.ptr)
	{
		__debugbreak();
		return FALSE;
	}

	return TRUE;
}

void SingleDescriptorAllocator::FreeDescriptorHandleForSRVUAV(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE base = _SRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
#ifdef _DEBUG
	if (base.ptr > descriptorHandle.ptr)
	{
		__debugbreak();
		return;
	}
#endif
	DWORD dwIndex = (DWORD)(descriptorHandle.ptr - base.ptr) / _SRVUAVDescSize;
	_indexCreatorForSRVUAV.Free(dwIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE SingleDescriptorAllocator::GetGPUHandleFromCPUHandleForSRVUAV(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE base = _SRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
#ifdef _DEBUG
	if (base.ptr > cpuHandle.ptr)
	{
		__debugbreak();
	}
#endif
	DWORD dwIndex = (DWORD)(cpuHandle.ptr - base.ptr) / _SRVUAVDescSize;
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(_SRVUAVDescHeap->GetGPUDescriptorHandleForHeapStart(), dwIndex, _SRVUAVDescSize);
	return gpuHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE SingleDescriptorAllocator::GetGPUHandleFromCPUHandleForRTV(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE base = _RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
#ifdef _DEBUG
	if (base.ptr > cpuHandle.ptr)
	{
		__debugbreak();
	}
#endif
	DWORD dwIndex = (DWORD)(cpuHandle.ptr - base.ptr) / _RTVDescSize;
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(_RTVDescHeap->GetGPUDescriptorHandleForHeapStart(), dwIndex, _RTVDescSize);
	return gpuHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE SingleDescriptorAllocator::GetGPUHandleFromCPUHandleForDSV(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE base = _DSVDescHeap->GetCPUDescriptorHandleForHeapStart();
#ifdef _DEBUG
	if (base.ptr > cpuHandle.ptr)
	{
		__debugbreak();
	}
#endif
	DWORD dwIndex = (DWORD)(cpuHandle.ptr - base.ptr) / _DSVDescSize;
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(_DSVDescHeap->GetGPUDescriptorHandleForHeapStart(), dwIndex, _DSVDescSize);
	return gpuHandle;
}

void SingleDescriptorAllocator::Cleanup()
{
#ifdef _DEBUG
	_indexCreatorForSRVUAV.Check();
	_indexCreatorForRTV.Check();
	_indexCreatorForDSV.Check();
#endif
}

SingleDescriptorAllocator::~SingleDescriptorAllocator()
{
	Cleanup();
}