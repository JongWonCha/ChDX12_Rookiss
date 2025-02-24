#include "pch.h"
#include "StructuredBuffer.h"
#include "Engine.h"
#include "SingleDescriptorAllocator.h"

StructuredBuffer::StructuredBuffer()
{
}

StructuredBuffer::~StructuredBuffer()
{
	if (srvHandle.ptr != 0)
		SINGLEDESCRIPTORALLOCATOR->FreeDescriptorHandleForSRVUAV(srvHandle);
	if (uavHandle.ptr != 0)
		SINGLEDESCRIPTORALLOCATOR->FreeDescriptorHandleForSRVUAV(uavHandle);
}

void StructuredBuffer::Init(uint32 elementSize, uint32 elementCount)
{
	_elementSize = elementSize;
	_elementCount = elementCount;
	_resourceState = D3D12_RESOURCE_STATE_COMMON;

	//buffer
	{
		uint64 bufferSize = static_cast<uint64>(_elementSize) * _elementCount;

		DEVICE->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
			_resourceState,
			nullptr,
			IID_PPV_ARGS(_buffer.GetAddressOf())
		);
	}

	// SRV
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = _elementCount;
		srvDesc.Buffer.StructureByteStride = _elementSize;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
		BOOL hr = SINGLEDESCRIPTORALLOCATOR->AllocDescriptorHandleForSRVUAV(&cpuHandle);
		if (!hr) __debugbreak();

		DEVICE->CreateShaderResourceView(_buffer.Get(), &srvDesc, cpuHandle);
		srvHandle = cpuHandle;
	}

	//UAV
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = _elementCount;
		uavDesc.Buffer.StructureByteStride = _elementSize;
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
		BOOL hr = SINGLEDESCRIPTORALLOCATOR->AllocDescriptorHandleForSRVUAV(&cpuHandle);
		if (!hr) __debugbreak();

		DEVICE->CreateUnorderedAccessView(_buffer.Get(), nullptr, &uavDesc, cpuHandle);
		uavHandle = cpuHandle;
	}
}

void StructuredBuffer::PushGraphicsData()
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};
	GRAPHICS_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);
	DEVICE->CopyDescriptorsSimple(1, cpuHandle, srvHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void StructuredBuffer::PushComputeSRVData(uint32 registerIndex)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};

	BOOL hr = COMPUTE_DESC_POOL->AllocDescriptorTableByIndex(registerIndex, &cpuHandle, &gpuHandle);

	if (!hr) __debugbreak();

	uint32 destRange = 1;
	uint32 srcRange = 1;
	DEVICE->CopyDescriptorsSimple(1, cpuHandle, srvHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void StructuredBuffer::PushComputeUAVData(uint32 registerIndex)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};

	BOOL hr = COMPUTE_DESC_POOL->AllocDescriptorTableByIndex(registerIndex, &cpuHandle, &gpuHandle);

	if (!hr) __debugbreak();

	uint32 destRange = 1;
	uint32 srcRange = 1;
	DEVICE->CopyDescriptorsSimple(1, cpuHandle, uavHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//DEVICE->CopyDescriptors(1, &cpuHandle, &destRange, 1, &uavHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
