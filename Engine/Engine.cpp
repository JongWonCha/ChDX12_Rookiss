#include "pch.h"
#include "Engine.h"
#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Transform.h"
#include "Material.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"


void Engine::Init(const WindowInfo& window)
{
	_window = window;

	_viewport = { 0, 0, static_cast<FLOAT>(window.width), static_cast<FLOAT>(window.height), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, window.width, window.height);

	_device = make_shared<Device>();
	_cmdQueue = make_shared<CommandQueue>();
	_swapChain = make_shared<SwapChain>();
	_rootSignature = make_shared<RootSignature>();
	_descriptorPool = make_shared<DescriptorPool>();
	_constantBuffers.emplace_back(make_shared<ConstantBuffer>()); //
	_constantBuffers.emplace_back(make_shared<ConstantBuffer>());
	//_constantBuffers = make_shared<ConstantBuffer>();
	_singleDescriptorAllocator = make_shared<SingleDescriptorAllocator>();
	_depthStencilBuffer = make_shared<DepthStencilBuffer>();

	_device->Init();
	_cmdQueue->Init(_device->GetDevice(), _swapChain);
	_swapChain->Init(window, _device->GetDevice(), _device->GetDXGI(), _cmdQueue->GetCmdQueue());
	_rootSignature->Init();
	_descriptorPool->Init(_device->GetDevice(), 256 * 2);
	_constantBuffers[0]->Init(sizeof(Constant_TransformMatrix), 256);
	_constantBuffers[1]->Init(sizeof(Constant_MaterialParams), 256 * 2);
	_singleDescriptorAllocator->Init(4096, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	_depthStencilBuffer->Init(window);

	GET_SINGLE(Input)->Init(window.hwnd);
	GET_SINGLE(Timer)->Init();

	ResizeWindow(window.width, window.height);
}

void Engine::Update()
{
	GET_SINGLE(Input)->Update();
	GET_SINGLE(Timer)->Update();

	Render();
}

void Engine::LateUpdate()
{
}

void Engine::RenderBegin()
{
	_cmdQueue->RenderBegin(&_viewport, &_scissorRect);
}

void Engine::Render()
{
	RenderBegin();

	GET_SINGLE(SceneManager)->Update();

	RenderEnd();
}

void Engine::RenderEnd()
{
	_cmdQueue->RenderEnd();
}

void Engine::ResizeWindow(int32 width, int32 height)
{
	_window.width = width;
	_window.height = height;

	RECT rect = { 0, 0, width, height };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	::SetWindowPos(_window.hwnd, 0, 100, 100, width, height, 0);

	_depthStencilBuffer->Init(_window);
}

