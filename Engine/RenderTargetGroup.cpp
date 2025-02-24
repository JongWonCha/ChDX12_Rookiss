#include "pch.h"
#include "RenderTargetGroup.h"
#include "Engine.h"
#include "Device.h"
#include "Texture.h"
#include "SingleDescriptorAllocator.h"

void RenderTargetGroup::Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec)
{
	_groupType = groupType;
	_rtVec = rtVec;
	_rtCount = static_cast<uint32>(rtVec.size());
	
	_rtvHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	int32 offset = 0;
	switch(groupType)
	{
	case RENDER_TARGET_GROUP_TYPE::G_BUFFER:
		offset = 2;
		break;
	case RENDER_TARGET_GROUP_TYPE::LIGHTING:
		offset = 5;
		break;
	case RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN:
	case RENDER_TARGET_GROUP_TYPE::DEPTH_STENCIL:
		return;
	}

	for (uint32 i = 0; i < _rtCount; ++i)
	{
		_targetToResource[i] = CD3DX12_RESOURCE_BARRIER::Transition(_rtVec[i].target->GetTextureHandle()->pTexResource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);

		_resourceToTarget[i] = CD3DX12_RESOURCE_BARRIER::Transition(_rtVec[i].target->GetTextureHandle()->pTexResource.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
}

void RenderTargetGroup::OMSetRenderTarget(uint32 count, uint32 offset)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorHeapStart(), offset, _rtvHeapSize);
	GRAPHICS_CMD_LIST->OMSetRenderTargets(count, &rtvHandle, FALSE/*1°³¸¸ »ç¿ë*/, &SINGLEDESCRIPTORALLOCATOR->GetDSVDescriptorHeapStart());
}

void RenderTargetGroup::OMSetRenderTargets(uint32 count, uint32 offset, uint32 index)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE gBufferRTVHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorHeapStart(), SWAP_CHAIN_BUFFER_COUNT + index * (RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT + RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT) + offset, _rtvHeapSize);

	GRAPHICS_CMD_LIST->OMSetRenderTargets(count, &gBufferRTVHandle, TRUE/*´ÙÁß ·»´õ Å¸°Ù*/, &SINGLEDESCRIPTORALLOCATOR->GetDSVDescriptorHeapStart());
}

void RenderTargetGroup::ClearRenderTargetView(uint32 index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorHeapStart(), index * _rtvHeapSize);
	GRAPHICS_CMD_LIST->ClearRenderTargetView(rtvHandle, _rtVec[index].clearColor, 0, nullptr);

	GRAPHICS_CMD_LIST->ClearDepthStencilView(SINGLEDESCRIPTORALLOCATOR->GetDSVDescriptorHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderTargetGroup::ClearRenderTargetViews(uint32 count, uint32 offset, uint32 index)
{
	WaitResourceToTarget();

	CD3DX12_CPU_DESCRIPTOR_HANDLE gBufferRTVHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorHeapStart(), SWAP_CHAIN_BUFFER_COUNT + index * (RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT + RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT) + offset, _rtvHeapSize);
	
	for (uint32 i = 0; i < count; ++i)
	{
		GRAPHICS_CMD_LIST->ClearRenderTargetView(gBufferRTVHandle, _rtVec[i].clearColor, 0, nullptr);
		gBufferRTVHandle.Offset(1, _rtvHeapSize);
	}
	//CMD_LIST->ClearDepthStencilView(SINGLEDESCRIPTORALLOCATOR->GetDSVDescriptorHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderTargetGroup::WaitTargetToResource()
{
	GRAPHICS_CMD_LIST->ResourceBarrier(_rtCount, _targetToResource);
}

void RenderTargetGroup::WaitResourceToTarget()
{
	GRAPHICS_CMD_LIST->ResourceBarrier(_rtCount, _resourceToTarget);
}
