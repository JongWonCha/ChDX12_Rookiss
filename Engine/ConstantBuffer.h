#pragma once
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

	void Init(uint32 size, uint32 count);

	void Reset();
	CB_CONTAINER * Alloc();

private:
	void CreateBuffer();

private:
	CB_CONTAINER*			_pCBContainerList = nullptr;

	ComPtr<ID3D12Resource>			_cbvBuffer;
	ComPtr<ID3D12DescriptorHeap>	_descriptorHeap = nullptr;
	UINT8*							_systemMemAddr = nullptr;
	uint32							_elementSize = 0;
	uint32							_elementCount = 0;

	uint32							_currentIndex = 0;
};

