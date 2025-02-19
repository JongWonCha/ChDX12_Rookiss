#include "pch.h"
#include "Engine.h"
#include "CommandQueue.h"
#include "SwapChain.h"

void CommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{
	_swapChain = swapChain;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));

	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));

	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));

	_cmdList->Close();

	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_resCmdAlloc.GetAddressOf()));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdAlloc.Get(), nullptr, IID_PPV_ARGS(_resCmdList.GetAddressOf()));

	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	//_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CommandQueue::WaitSync()
{
	_fenceValue++;

	_cmdQueue->Signal(_fence.Get(), _fenceValue);

	if (_fence->GetCompletedValue() < _fenceValue)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		_fence->SetEventOnCompletion(_fenceValue, eventHandle);

		::WaitForSingleObject(eventHandle, INFINITE);
		::CloseHandle(eventHandle);
	}
}

void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);

	
	_cmdList->RSSetViewports(1, vp);
	_cmdList->RSSetScissorRects(1, rect);

	//_cmdList->SetGraphicsRootSignature(ROOT_SIGNATURE.Get());
	shared_ptr<Texture> backBuffer = 
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(_swapChain->GetBackBufferIndex());

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffer->GetTextureHandle()->pTexResource.Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	_cmdList->ResourceBarrier(1, &barrier);


	/*D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = backBuffer->GetTextureHandle()->rtvDesc;
	const float BackColor[] = { 0.f, 0.f, 0.f, 1.0f };
	_cmdList->ClearRenderTargetView(backBufferView, BackColor, 0, nullptr);
	
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::DEPTH_STENCIL)->GetRTTexture(0)->GetTextureHandle()->dsvDesc;
	_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, &depthStencilView);
	_cmdList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);*/

	_cmdList->SetGraphicsRootSignature(ROOT_SIGNATURE.Get());
	_cmdList->SetDescriptorHeaps(1, DESCRIPTORPOOL->GetDescriptorHeap().GetAddressOf());

}

void CommandQueue::RenderEnd()
{
	shared_ptr<Texture> backBuffer = 
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(_swapChain->GetBackBufferIndex());

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffer->GetTextureHandle()->pTexResource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	_cmdList->ResourceBarrier(1, &barrier);
	_cmdList->Close();

	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	_swapChain->Present();

	WaitSync();

	_swapChain->SwapIndex();
}

void CommandQueue::FlushResourceCommandQueue()
{
	_resCmdList->Close();

	ID3D12CommandList* ppCmdListArr[] = { _resCmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(ppCmdListArr), ppCmdListArr);

	WaitSync();

	_resCmdAlloc->Reset();
	_resCmdList->Reset(_resCmdAlloc.Get(), nullptr);
}

CommandQueue::~CommandQueue()
{
	::CloseHandle(_fenceEvent);
}