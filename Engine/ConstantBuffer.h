#pragma once

enum class CONSTANT_BUFFER_TYPE : uint8
{
	LIGHT = 0,
	TRANSFORM,
	MATERIAL,
	END
};

enum
{
	CONSTANT_BUFFER_COUNT = static_cast<uint8>(CONSTANT_BUFFER_TYPE::END)
};

struct CB_CONTAINER
{
	D3D12_CPU_DESCRIPTOR_HANDLE	CBVHandle;
	D3D12_GPU_VIRTUAL_ADDRESS	pGPUMemAddr;
	UINT8*						pSystemMemAddr;
};

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	void Init(uint32 elementSize, uint32 elementCount);

	void Reset();
	void SetGlobalData(void* buffer, uint32 size);
	CB_CONTAINER * Alloc();

private:
	void CreateBuffer();

private:
	CB_CONTAINER*					_pCBContainerList = nullptr;

	ComPtr<ID3D12Resource>			_cbvBuffer;
	ComPtr<ID3D12DescriptorHeap>	_descriptorHeap = nullptr;
	BYTE*							_systemMemAddr = nullptr;
	uint32							_elementSize = 0;
	uint32							_elementCount = 0;

	uint32							_currentIndex = 0;
};

