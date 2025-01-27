#pragma once

class Device;
class CommandQueue;
class SwapChain;
class DescriptorHeap;

class Engine
{
public:

	void Init(const WindowInfo& window);
	void Render();

	void RenderBegin();
	void RenderEnd();

	void ResizeWindow(int32 width, int32 height);
private:
	WindowInfo		_window;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};

	shared_ptr<Device>			_device;
	shared_ptr<CommandQueue>	_cmdQueue;
	shared_ptr<SwapChain>		_swapChain;
	shared_ptr<DescriptorHeap>	_descHeap;
};

