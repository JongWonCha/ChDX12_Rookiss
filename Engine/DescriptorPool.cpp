#include "pch.h"
#include "DescriptorPool.h"

DescriptorPool::DescriptorPool()
{
}

BOOL DescriptorPool::Init(ComPtr<ID3D12Device> device, UINT maxDescriptorCount)
{
    _device = device;

    _maxDescriptorCount = maxDescriptorCount;
    _srvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_DESCRIPTOR_HEAP_DESC commonHeapDesc = {};
    commonHeapDesc.NumDescriptors = _maxDescriptorCount;
    commonHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    commonHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    if (FAILED(_device->CreateDescriptorHeap(&commonHeapDesc, IID_PPV_ARGS(&_descriptorHeap))))
    {
        __debugbreak();
        return FALSE;
    }
    _cpuDescriptorHandle = _descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    _gpuDescriptorHandle = _descriptorHeap->GetGPUDescriptorHandleForHeapStart();
}

BOOL DescriptorPool::AllocDescriptorTable(D3D12_CPU_DESCRIPTOR_HANDLE* pOutCPUDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE* pOutGPUDescriptor, UINT descriptorCount)
{
    if (_allocatedDescriptorCount + descriptorCount > _maxDescriptorCount) return FALSE;

    *pOutCPUDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuDescriptorHandle, _allocatedDescriptorCount, _srvDescriptorSize);
    *pOutGPUDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(_gpuDescriptorHandle, _allocatedDescriptorCount, _srvDescriptorSize);
    _allocatedDescriptorCount += descriptorCount;

    return TRUE;
}

void DescriptorPool::Reset()
{
    _allocatedDescriptorCount = 0;
}

DescriptorPool::~DescriptorPool()
{
    
}
