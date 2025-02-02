#pragma once
class DescriptorPool
{
public:
	BOOL Init(ComPtr<ID3D12Device> device, UINT maxDescriptorCount);

	BOOL AllocDescriptorTable(D3D12_CPU_DESCRIPTOR_HANDLE* pOutCPUDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE* pOutGPUDescriptor, UINT descriptorCount);
	void Reset();
	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return _descriptorHeap; }
	DescriptorPool();
	~DescriptorPool();

private:
	ComPtr<ID3D12Device> _device = nullptr;
	UINT				_allocatedDescriptorCount = 0;
	UINT				_maxDescriptorCount = 0;
	UINT				_srvDescriptorSize = 0;
	ComPtr<ID3D12DescriptorHeap>	_descriptorHeap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE _cpuDescriptorHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE	_gpuDescriptorHandle = {};

	void Cleanup();
};

