#include "pch.h"
#include "Engine.h"
#include "IndexCreator.h"
#include "SingleDescriptorAllocator.h"

SingleDescriptorAllocator::SingleDescriptorAllocator()
{
}

BOOL SingleDescriptorAllocator::Init(DWORD maxCount, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
	D3D12_DESCRIPTOR_HEAP_DESC commonHeapDesc = {};
	commonHeapDesc.NumDescriptors = maxCount;
	commonHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	commonHeapDesc.Flags = flags;

	if (FAILED(DEVICE->CreateDescriptorHeap(&commonHeapDesc, IID_PPV_ARGS(&_descHeap))))
	{
		__debugbreak();
		return FALSE;
	}

	_indexCreator.Initialize(maxCount);

	_descSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return TRUE;
}

BOOL SingleDescriptorAllocator::AllocDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* outDescriptorHandle)
{
	DWORD dwIndex = _indexCreator.Alloc();
	if (dwIndex != -1)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(_descHeap->GetCPUDescriptorHandleForHeapStart(), dwIndex, _descSize);
		*outDescriptorHandle = descriptorHandle;
		return TRUE;
	}
	return FALSE;
}

BOOL SingleDescriptorAllocator::Check(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE base = _descHeap->GetCPUDescriptorHandleForHeapStart();
	if (base.ptr > descriptorHandle.ptr)
	{
		__debugbreak();
		return FALSE;
	}

	return TRUE;
}

void SingleDescriptorAllocator::FreeDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE base = _descHeap->GetCPUDescriptorHandleForHeapStart();
#ifdef _DEBUG
	if (base.ptr > descriptorHandle.ptr)
	{
		__debugbreak();
		return;
	}
#endif
	DWORD dwIndex = (DWORD)(descriptorHandle.ptr - base.ptr) / _descSize;
	_indexCreator.Free(dwIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE SingleDescriptorAllocator::GetGPUHandleFromCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE base = _descHeap->GetCPUDescriptorHandleForHeapStart();
#ifdef _DEBUG
	if (base.ptr > cpuHandle.ptr)
	{
		__debugbreak();
	}
#endif
	DWORD dwIndex = (DWORD)(cpuHandle.ptr - base.ptr) / _descSize;
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(_descHeap->GetGPUDescriptorHandleForHeapStart(), dwIndex, _descSize);
	return gpuHandle;
}

void SingleDescriptorAllocator::Cleanup()
{
#ifdef _DEBUG
	_indexCreator.Check();
#endif
}

SingleDescriptorAllocator::~SingleDescriptorAllocator()
{
	Cleanup();
}