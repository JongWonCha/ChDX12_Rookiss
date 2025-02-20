#include "pch.h"
#include "RenderTargetGroup.h"
#include "Engine.h"
#include "Device.h"+
#include "SingleDescriptorAllocator.h"

void RenderTargetGroup::Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec)
{
	_groupType = groupType;
	_rtVec = rtVec;
	_rtCount = static_cast<uint32>(rtVec.size());
	
	_rtvHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void RenderTargetGroup::OMSetRenderTarget(uint32 count, uint32 offset)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorHeapStart(), offset, _rtvHeapSize);
	CMD_LIST->OMSetRenderTargets(count, &rtvHandle, FALSE/*1°³¸¸ »ç¿ë*/, &SINGLEDESCRIPTORALLOCATOR->GetDSVDescriptorHeapStart());
}

void RenderTargetGroup::OMSetRenderTargets(uint32 count, uint32 offset, uint32 index)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE gBufferRTVHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorHeapStart(), SWAP_CHAIN_BUFFER_COUNT + index * (RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT + RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT) + offset, _rtvHeapSize);

	CMD_LIST->OMSetRenderTargets(count, &gBufferRTVHandle, TRUE/*´ÙÁß ·»´õ Å¸°Ù*/, &SINGLEDESCRIPTORALLOCATOR->GetDSVDescriptorHeapStart());
}

void RenderTargetGroup::ClearRenderTargetView(uint32 index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorHeapStart(), index * _rtvHeapSize);
	CMD_LIST->ClearRenderTargetView(rtvHandle, _rtVec[index].clearColor, 0, nullptr);

	CMD_LIST->ClearDepthStencilView(SINGLEDESCRIPTORALLOCATOR->GetDSVDescriptorHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderTargetGroup::ClearRenderTargetViews(uint32 count, uint32 offset, uint32 index)
{
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE gBufferRTVHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorHeapStart(), SWAP_CHAIN_BUFFER_COUNT + index * (RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT + RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT) + offset, _rtvHeapSize);
	
	for (uint32 i = 0; i < count; ++i)
	{
		CMD_LIST->ClearRenderTargetView(gBufferRTVHandle, _rtVec[i].clearColor, 0, nullptr);
		gBufferRTVHandle.Offset(1, _rtvHeapSize);
	}
	//CMD_LIST->ClearDepthStencilView(SINGLEDESCRIPTORALLOCATOR->GetDSVDescriptorHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}
