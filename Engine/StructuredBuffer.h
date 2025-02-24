#pragma once
class StructuredBuffer
{
public:
	StructuredBuffer();
	~StructuredBuffer();

	void Init(uint32 elementSize, uint32 elementCount);

	void PushGraphicsData();
	void PushComputeSRVData(uint32 registerIndex);
	void PushComputeUAVData(uint32 registerIndex);

	void SetResourceState(D3D12_RESOURCE_STATES state) { _resourceState = state; }
	D3D12_RESOURCE_STATES GetResourceState() { return _resourceState; }
	ComPtr<ID3D12Resource> GetBuffer() { return _buffer; }

private:
	ComPtr<ID3D12Resource> _buffer;
	uint32 _elementSize = 0;
	uint32 _elementCount = 0;
	D3D12_RESOURCE_STATES _resourceState = {};

private:
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = {};
	D3D12_CPU_DESCRIPTOR_HANDLE uavHandle = {};
};

