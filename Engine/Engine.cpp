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
#include "Resources.h"


void Engine::Init(const WindowInfo& window)
{
	_window = window;

	_viewport = { 0, 0, static_cast<FLOAT>(window.width), static_cast<FLOAT>(window.height), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, window.width, window.height);

	_device = make_shared<Device>();
	_graphicsCmdQueue = make_shared<GraphicsCommandQueue>();
	_computeCmdQueue = make_shared<ComputeCommandQueue>();
	_swapChain = make_shared<SwapChain>();
	_rootSignature = make_shared<RootSignature>();

	_graphicsDescPool = make_shared<GraphicsDescriptorPool>();
	_computeDescPool = make_shared<ComputeDescriptorPool>();

	_constantBuffers.emplace_back(make_shared<ConstantBuffer>()); // lightParams
	_constantBuffers.emplace_back(make_shared<ConstantBuffer>()); // TransformParams
	_constantBuffers.emplace_back(make_shared<ConstantBuffer>()); // MaterialParams

	_singleDescriptorAllocator = make_shared<SingleDescriptorAllocator>();

	_device->Init();
	_graphicsCmdQueue->Init(_device->GetDevice(), _swapChain);
	_computeCmdQueue->Init(_device->GetDevice());
	_swapChain->Init(window, _device->GetDevice(), _device->GetDXGI(), _graphicsCmdQueue->GetCmdQueue());
	_rootSignature->Init();
	_graphicsDescPool->Init(_device->GetDevice(), 256 * 9); // 오브젝트 갯수 * 오브젝트 당 넘겨줄 레지스터 갯수(b1 ~ b4, t0 ~ t4)
	_computeDescPool->Init(_device->GetDevice(), 20 * 5);
	_constantBuffers[0]->Init(sizeof(Constant_LightParams), 1);
	_constantBuffers[1]->Init(sizeof(Constant_TransformParams), 256);
	_constantBuffers[2]->Init(sizeof(Constant_MaterialParams), 256);
	_singleDescriptorAllocator->Init(4096, SWAP_CHAIN_BUFFER_COUNT * (1 + RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT + RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT), 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	//_depthStencilBuffer->Init(window);

	CreateRenderTargetGroups();

	GET_SINGLE(Input)->Init(window.hwnd);
	GET_SINGLE(Timer)->Init();
	GET_SINGLE(Resources)->Init();

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
	_graphicsCmdQueue->RenderBegin(&_viewport, &_scissorRect);
}

void Engine::Render()
{
	RenderBegin();

	GET_SINGLE(SceneManager)->Render();

	RenderEnd();
}

void Engine::RenderEnd()
{
	_graphicsCmdQueue->RenderEnd();
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
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->GetRTTexture(i)->GetTextureHandle()->pTexResource.Reset();
	}
	
	_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::DEPTH_STENCIL)]->GetRTTexture(0)->GetTextureHandle()->pTexResource.Reset();
	//_depthStencilBuffer->GetDSVBuffer().Reset();

	GET_SINGLE(Resources)->Remove<Texture>(L"DepthStencil");

	hr = _swapChain->GetSwapChain()->ResizeBuffers(
		SWAP_CHAIN_BUFFER_COUNT,
		width, height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	);

	if (FAILED(hr))
		__debugbreak();

	_swapChain->SetBackBufferIndex(0);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorHeapStart());
	//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(_swapChain->GetRTVHeap()->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
	{
		_swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&(_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->GetRTTexture(i)->GetTextureHandle()->pTexResource)));
		_device->GetDevice()->CreateRenderTargetView(_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->GetRTTexture(i)->GetTextureHandle()->pTexResource.Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorSize());
	}
	
	/*_cmdQueue->GetCmdList()->Close();
	_cmdQueue->GetResourceCmdList()->Close();
	_cmdQueue->GetCmdList()->Reset(_cmdQueue->GetCmdAlloc().Get(), nullptr);
	_cmdQueue->GetResourceCmdList()->Reset(_cmdQueue->GetResouceCmdAlloc().Get(), nullptr);
	_cmdQueue->WaitSync();*/

	_window.width = width;
	_window.height = height;

	/*vector<RenderTarget>	dsVec(1);
	dsVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"DepthStencil", DXGI_FORMAT_D32_FLOAT, width, height,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::DEPTH_STENCIL)]->Create(RENDER_TARGET_GROUP_TYPE::DEPTH_STENCIL, dsVec);*/

	//_depthStencilBuffer->Init(_window);

	_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::DEPTH_STENCIL)]->ClearRTVector();
	vector<RenderTarget>	dsVec(1);
	dsVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"DepthStencil", DXGI_FORMAT_D32_FLOAT, _window.width, _window.height,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::DEPTH_STENCIL)]->Create(RENDER_TARGET_GROUP_TYPE::DEPTH_STENCIL, dsVec);

	_scissorRect = CD3DX12_RECT(0, 0, width, height);
	_viewport = { 0, 0, static_cast<FLOAT>(width), static_cast<FLOAT>(height), 0.0f, 1.0f };
}

void Engine::CreateRenderTargetGroups()
{
	//DepthStencilBuffer
	vector<RenderTarget>	dsVec(1);
	dsVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"DepthStencil", DXGI_FORMAT_D32_FLOAT, _window.width, _window.height,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::DEPTH_STENCIL)] = make_shared<RenderTargetGroup>();
	_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::DEPTH_STENCIL)]->Create(RENDER_TARGET_GROUP_TYPE::DEPTH_STENCIL, dsVec);
	

	// Swap Chain Group
	// SingleDescriptorAllocator에 가장 먼저 등록되어야 함(RTV 부분)
	{
		vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT);

		for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
		{
			wstring name = L"SwapChainTarget_" + std::to_wstring(i);

			ComPtr<ID3D12Resource> rtResource;
			float clearColor[4] = {1.f, 1.f, 1.f, 1.f};
			_swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&rtResource));
			rtVec[i].target = GET_SINGLE(Resources)->CreateTextureFromResource(name, rtResource);
			//rtVec[i].clearColor = clearColor;
		}

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->Create(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN, rtVec);
	}

	// Deffered Group + DeferredLighting Group
	{
		vector<RenderTarget> rtDeferredVec(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT * SWAP_CHAIN_BUFFER_COUNT);
		vector<RenderTarget> rtLightingVec(RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT * SWAP_CHAIN_BUFFER_COUNT);
		
		for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
		{
			rtDeferredVec[(i * RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT) + 0].target = GET_SINGLE(Resources)->CreateTexture(L"PositionTarget_" + std::to_wstring(i),
				DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
				CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

			rtDeferredVec[(i * RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT) + 1].target = GET_SINGLE(Resources)->CreateTexture(L"NormalTarget_" + std::to_wstring(i),
				DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
				CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

			rtDeferredVec[(i * RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT) + 2].target = GET_SINGLE(Resources)->CreateTexture(L"DiffuseTarget_" + std::to_wstring(i),
				DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
				CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

			rtLightingVec[i * RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT + 0].target = GET_SINGLE(Resources)->CreateTexture(L"DiffuseLightTarget_" + std::to_wstring(i),
				DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
				CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

			rtLightingVec[i * RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT + 1].target = GET_SINGLE(Resources)->CreateTexture(L"SpecularLightTarget_" + std::to_wstring(i),
				DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
				CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		}

		

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(RENDER_TARGET_GROUP_TYPE::G_BUFFER, rtDeferredVec);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)]->Create(RENDER_TARGET_GROUP_TYPE::LIGHTING, rtLightingVec);
	}

}

