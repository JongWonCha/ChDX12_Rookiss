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
#include "Camera.h"
#include "Light.h"


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
	_constantBuffers.emplace_back(make_shared<ConstantBuffer>()); // lightParams
	_constantBuffers.emplace_back(make_shared<ConstantBuffer>()); // TransformParams
	_constantBuffers.emplace_back(make_shared<ConstantBuffer>()); // MaterialParams
	//_constantBuffers = make_shared<ConstantBuffer>();
	_singleDescriptorAllocator = make_shared<SingleDescriptorAllocator>();
	_depthStencilBuffer = make_shared<DepthStencilBuffer>();

	_device->Init();
	_cmdQueue->Init(_device->GetDevice(), _swapChain);
	_swapChain->Init(window, _device->GetDevice(), _device->GetDXGI(), _cmdQueue->GetCmdQueue());
	_rootSignature->Init();
	_descriptorPool->Init(_device->GetDevice(), 256 * 9); // 오브젝트 갯수 * 오브젝트 당 넘겨줄 레지스터 갯수(b1 ~ b4, t0 ~ t4)
	_constantBuffers[0]->Init(sizeof(Constant_LightParams), 1);
	_constantBuffers[1]->Init(sizeof(Constant_TransformParams), 256);
	_constantBuffers[2]->Init(sizeof(Constant_MaterialParams), 256);
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
	GET_SINGLE(SceneManager)->Update();

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

	GET_SINGLE(SceneManager)->Render();

	RenderEnd();
}

void Engine::RenderEnd()
{
	_cmdQueue->RenderEnd();
}

void Engine::ResizeWindow(int32 width, int32 height)
{
	assert(_device->GetDevice());
	assert(_swapChain->GetSwapChain());

	if (!(width * height)) return;

	if ((_window.width == width) && (_window.height == height)) return;

	DXGI_SWAP_CHAIN_DESC desc;
	HRESULT hr = _swapChain->GetSwapChain()->GetDesc(&desc);

	if (FAILED(hr))
		__debugbreak();

	for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
	{
		_swapChain->GetRTVBuffer(i).Reset();
	}
	_depthStencilBuffer->GetDSVBuffer().Reset();

	hr = _swapChain->GetSwapChain()->ResizeBuffers(
		SWAP_CHAIN_BUFFER_COUNT,
		width, height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	);

	if (FAILED(hr))
		__debugbreak();

	_swapChain->SetBackBufferIndex(0);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(_swapChain->GetRTVHeap()->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
	{
		_swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&_swapChain->GetRTVBuffer(i)));
		_device->GetDevice()->CreateRenderTargetView(_swapChain->GetRTVBuffer(i).Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, _swapChain->GetRTVElementSize());
	}
	
	/*_cmdQueue->GetCmdList()->Close();
	_cmdQueue->GetResourceCmdList()->Close();
	_cmdQueue->GetCmdList()->Reset(_cmdQueue->GetCmdAlloc().Get(), nullptr);
	_cmdQueue->GetResourceCmdList()->Reset(_cmdQueue->GetResouceCmdAlloc().Get(), nullptr);
	_cmdQueue->WaitSync();*/

	_window.width = width;
	_window.height = height;

	_depthStencilBuffer->Init(_window);

	_scissorRect = CD3DX12_RECT(0, 0, width, height);
	_viewport = { 0, 0, static_cast<FLOAT>(width), static_cast<FLOAT>(height), 0.0f, 1.0f };
}

