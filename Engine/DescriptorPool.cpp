#include "pch.h"
#include "DescriptorPool.h"

GraphicsDescriptorPool::GraphicsDescriptorPool()
{
}

BOOL GraphicsDescriptorPool::Init(ComPtr<ID3D12Device> device, UINT maxDescriptorCount)
{
    _device = device;

    _maxDescriptorCount = maxDescriptorCount;
    _srvDescSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_DESCRIPTOR_HEAP_DESC commonHeapDesc = {};
    commonHeapDesc.NumDescriptors = _maxDescriptorCount;
    commonHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    commonHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    if (FAILED(_device->CreateDescriptorHeap(&commonHeapDesc, IID_PPV_ARGS(&_descHeap))))
    {
        __debugbreak();
        return FALSE;
    }
    _cpuDescriptorHandle = _descHeap->GetCPUDescriptorHandleForHeapStart();
    _gpuDescriptorHandle = _descHeap->GetGPUDescriptorHandleForHeapStart();
}

BOOL GraphicsDescriptorPool::AllocDescriptorTable(D3D12_CPU_DESCRIPTOR_HANDLE* pOutCPUDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE* pOutGPUDescriptor, UINT descriptorCount)
{
    if (_allocatedDescriptorCount + descriptorCount > _maxDescriptorCount)
    {
        __debugbreak();
        return FALSE;
    }

    *pOutCPUDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuDescriptorHandle, _allocatedDescriptorCount, _srvDescSize);
    *pOutGPUDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(_gpuDescriptorHandle, _allocatedDescriptorCount, _srvDescSize);
    _allocatedDescriptorCount += descriptorCount;

    return TRUE;
}

GraphicsDescriptorPool::~GraphicsDescriptorPool()
{

}

//*********************
//ComputeDescriptorPool
//**********************

void GraphicsDescriptorPool::Reset()
{
    _allocatedDescriptorCount = 0;
}

BOOL ComputeDescriptorPool::Init(ComPtr<ID3D12Device> device, UINT maxCount)
{
    _device = device;

    _maxDescCount = maxCount;
    _srvDescSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_DESCRIPTOR_HEAP_DESC commonHeapDesc = {};
    commonHeapDesc.NumDescriptors = _maxDescCount;
    commonHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    commonHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    if (FAILED(_device->CreateDescriptorHeap(&commonHeapDesc, IID_PPV_ARGS(&_descHeap))))
    {
        __debugbreak();
        return FALSE;
    }
    _cpuHandle = _descHeap->GetCPUDescriptorHandleForHeapStart();
    _gpuHandle = _descHeap->GetGPUDescriptorHandleForHeapStart();
}

BOOL ComputeDescriptorPool::AllocDescriptorTable(D3D12_CPU_DESCRIPTOR_HANDLE* pOutCPUDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE* pOutGPUDescriptor, UINT descriptorCount)
{
    if (_allocatedDescCount + descriptorCount > _maxDescCount)
    {
        __debugbreak();
        return FALSE;
    }

    *pOutCPUDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandle, _allocatedDescCount, _srvDescSize);
    *pOutGPUDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(_gpuHandle, _allocatedDescCount, _srvDescSize);
    _allocatedDescCount += descriptorCount;

    return TRUE;
}

BOOL ComputeDescriptorPool::AllocDescriptorTableByIndex(uint32 index, D3D12_CPU_DESCRIPTOR_HANDLE* pOutCPUDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE* pOutGPUDescriptor)
{
    if (index + 1 > _maxDescCount) return FALSE;

    *pOutCPUDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandle, index, _srvDescSize);
    *pOutGPUDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(_gpuHandle, index, _srvDescSize);

    return TRUE;
}



void ComputeDescriptorPool::Reset()
{
    _allocatedDescCount = 0;
}

ComputeDescriptorPool::ComputeDescriptorPool()
{
}

ComputeDescriptorPool::~ComputeDescriptorPool()
{
}
