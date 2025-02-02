#include "pch.h"
#include "ConstantBuffer.h"
#include "Engine.h"

ConstantBuffer::ConstantBuffer()
{
}

ConstantBuffer::~ConstantBuffer()
{
	if (_cbvBuffer)
	{
		if (_cbvBuffer != nullptr)
			_cbvBuffer->Unmap(0, nullptr);

		_cbvBuffer = nullptr;
	}

	if (_pCBContainerList)
	{
		delete[] _pCBContainerList;
		_pCBContainerList = nullptr;
	}
}

void ConstantBuffer::Init(uint32 size, uint32 count)
{
	_elementSize = (size + 255) & ~255;
	_elementCount = count;

	CreateBuffer();
}

void ConstantBuffer::CreateBuffer()
{
	uint32 bufferSize = _elementSize * _elementCount;
	
	DEVICE->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_cbvBuffer)
	);

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = _elementCount;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	
	if (FAILED(DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_descriptorHeap))))
	{
		__debugbreak();
	}

	CD3DX12_RANGE writeRange(0, 0);
	_cbvBuffer->Map(0, &writeRange, reinterpret_cast<void**>(&_systemMemAddr));

	_pCBContainerList = new CB_CONTAINER[_elementCount];

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _cbvBuffer.Get()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = _elementSize;

	UINT8* pSystemMemAddr = _systemMemAddr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE heapHandle(_descriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart());

	UINT descriptorSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (DWORD i = 0; i < _elementCount; i++)
	{
		DEVICE->CreateConstantBufferView(&cbvDesc, heapHandle);

		_pCBContainerList[i].CBVHandle = heapHandle;
		_pCBContainerList[i].pGPUMemAddr = cbvDesc.BufferLocation;
		_pCBContainerList[i].pSystemMemAddr = pSystemMemAddr;

		heapHandle.Offset(1, descriptorSize);
		cbvDesc.BufferLocation += _elementSize;
		pSystemMemAddr += _elementSize;
	}
}

void ConstantBuffer::Reset()
{
	_currentIndex = 0;
}

CB_CONTAINER* ConstantBuffer::Alloc()
{
	CB_CONTAINER* pCB = nullptr;

	if (_currentIndex >= _elementCount) return nullptr;

	pCB = _pCBContainerList + _currentIndex;
	_currentIndex++;

	return pCB;
}
