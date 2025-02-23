#pragma once
// 셰이더에게 제출할 통합 디스크립터 힙 용도.
// ex)
// obj0 (b0, b1, t0), obj1 (b0, t0) 을 그려내야 한다면?
// b0 | b1 | t0 || b0 | t0의 형태로 복사 될 것임
// SetDescriptorHeaps 호출 최소화의 목적, 디스크립터 힙의 생성 최소화의 목적으로 만들어짐.
class GraphicsDescriptorPool
{
public:
	BOOL Init(ComPtr<ID3D12Device> device, UINT maxDescriptorCount);

	BOOL AllocDescriptorTable(D3D12_CPU_DESCRIPTOR_HANDLE* pOutCPUDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE* pOutGPUDescriptor, UINT descriptorCount);
	void Reset();
	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return _descHeap; }
	UINT GetSrvDescirptorSize() { return _srvDescSize; }
	GraphicsDescriptorPool();
	~GraphicsDescriptorPool();

private:
	ComPtr<ID3D12Device> _device = nullptr;
	UINT				_allocatedDescriptorCount = 0;
	UINT				_maxDescriptorCount = 0;
	UINT				_srvDescSize = 0;
	ComPtr<ID3D12DescriptorHeap>	_descHeap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE _cpuDescriptorHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE	_gpuDescriptorHandle = {};

	void Cleanup();
};

//*********************
//ComputeDescriptorPool
//*********************

class ComputeDescriptorPool
{
private:
	ComPtr<ID3D12Device>			_device;
	UINT							_allocatedDescCount = 0;
	UINT							_maxDescCount;
	UINT							_srvDescSize = 0;
	ComPtr<ID3D12DescriptorHeap>	_descHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE		_cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE		_gpuHandle = {};

public:
	BOOL Init(ComPtr<ID3D12Device> device, UINT maxCount);

	BOOL AllocDescriptorTable(D3D12_CPU_DESCRIPTOR_HANDLE* pOutCPUDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE* pOutGPUDescriptor, UINT descriptorCount);
	void Reset();
	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return _descHeap; }
	UINT GetSrvDescirptorSize() { return _srvDescSize; }
	ComputeDescriptorPool();
	~ComputeDescriptorPool();
};

