#pragma once

#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "Mesh.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "DescriptorPool.h"
#include "Texture.h"
#include "SingleDescriptorAllocator.h"
#include "DepthStencilBuffer.h"

#include "Input.h"
#include "Timer.h"


class Engine
{
public:

	void Init(const WindowInfo& window);
	void Render();

public:
	void Update();

public:
	shared_ptr<Device> GetDevice() { return _device; }
	shared_ptr<CommandQueue> GetCmdQueue() { return _cmdQueue; }
	shared_ptr<SwapChain> GetSwapChain() { return _swapChain; }
	shared_ptr<RootSignature> GetRootSignature() { return _rootSignature; }
	shared_ptr<ConstantBuffer> GetCBByType(CONSTANT_BUFFER_TYPE type) { return _constantBuffers[static_cast<uint8>(type)]; }
	shared_ptr<DescriptorPool> GetDescriptorPool() { return _descriptorPool; }
	shared_ptr<SingleDescriptorAllocator> GetSingleDescriptorAllocator() { return _singleDescriptorAllocator; }
	shared_ptr<DepthStencilBuffer> GetDepthStencilBuffer() { return _depthStencilBuffer; }

	shared_ptr<Input> GetInput() { return _input; }
	shared_ptr<Timer> GetTimer() { return _timer; }

	void RenderBegin();
	void RenderEnd();

	void ResizeWindow(int32 width, int32 height);

private:
	WindowInfo		_window;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};

	shared_ptr<Device>						_device;
	shared_ptr<CommandQueue>				_cmdQueue;
	shared_ptr<SwapChain>					_swapChain;
	shared_ptr<RootSignature>				_rootSignature;
	vector<shared_ptr<ConstantBuffer>>		_constantBuffers;
	shared_ptr<DescriptorPool>				_descriptorPool;
	shared_ptr<SingleDescriptorAllocator>	_singleDescriptorAllocator;
	shared_ptr<DepthStencilBuffer>			_depthStencilBuffer;

	shared_ptr<Input>						_input;
	shared_ptr<Timer>						_timer;
};

