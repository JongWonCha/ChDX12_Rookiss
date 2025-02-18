#pragma once
#include "Engine.h"
class SwapChain
{
public:
	void Init(const WindowInfo& window, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	void Present();
	void SwapIndex();

	ComPtr<IDXGISwapChain>& GetSwapChain() { return _swapChain; }
	/*ComPtr<ID3D12Resource>& GetRTVBuffer(int32 index) { return _rtvBuffer[index]; }
	D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandle(int32 index) { return _rtvHandle[index]; }

	ComPtr<ID3D12Resource>& GetCurrentBackRTVBuffer() { return _rtvBuffer[_backBufferIndex]; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetBackRTV() const { return _rtvHandle[_backBufferIndex]; }

	ComPtr<ID3D12DescriptorHeap>& GetRTVHeap() { return _rtvHeap; }

	uint32 GetRTVElementSize() { return _rtvHeapSize; }*/

	//ComPtr<ID3D12Resource>& GetCurrentBackBuffer() { return  }

	void SetBackBufferIndex(uint32 index) { _backBufferIndex = index; }
	uint8 GetBackBufferIndex() { return _backBufferIndex; }

private:
	void CreateSwapChain(const WindowInfo& window, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	void CreateRTV(ComPtr<ID3D12Device> device);
private:
	//ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	//uint32							_rtvHeapSize = 0;
	//D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle[SWAP_CHAIN_BUFFER_COUNT];	// rtv 리소스를 가리키는 디스크립터

	ComPtr<IDXGISwapChain>			_swapChain;
	//ComPtr<ID3D12Resource>			_rtvBuffer[SWAP_CHAIN_BUFFER_COUNT];
	uint32							_backBufferIndex = 0;
};

