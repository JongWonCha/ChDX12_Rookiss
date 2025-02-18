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
	//_dsTexture = dsTexture;

	//D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
	//HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	//HeapDesc.NumDescriptors = _rtCount;
	//HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; //
	//HeapDesc.NodeMask = 0;

	//DEVICE->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&_rtvHeap));

	_rtvHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//_rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
	////_dsvHeapBegin = _dsTexture->GetDSV()->GetCPUDescriptorHandleForHeapStart();
	//_dsvHeapBegin = SINGLEDESCRIPTORALLOCATOR->GetDSVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

	/*for (uint32 i = 0; i < _rtCount; ++i)
	{
		uint32 destSize = 1;
		D3D12_CPU_DESCRIPTOR_HANDLE destHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeapBegin, i * _rtvHeapSize);

		uint32 srvSize = 1;
		ComPtr<ID3D12DescriptorHeap> srcRtvHeapBegin = _rtVec[i].target->GetRTV();
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srcRtvHeapBegin->GetCPUDescriptorHandleForHeapStart();

		DEVICE->CopyDescriptors(1, &destHandle, &destSize, 1, &srvHandle, &srvSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}*/

}

void RenderTargetGroup::OMSetRenderTargets(uint32 count, uint32 offset)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorHeapStart(), offset * _rtvHeapSize);
	CMD_LIST->OMSetRenderTargets(count, &rtvHandle, FALSE/*1°³¸¸ »ç¿ë*/, &SINGLEDESCRIPTORALLOCATOR->GetDSVDescriptorHeapStart());
}

void RenderTargetGroup::OMSetRenderTargets()
{
	CMD_LIST->OMSetRenderTargets(_rtCount, &(SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorHeapStart()), FALSE/*´ÙÁß ·»´õ Å¸°Ù*/, &SINGLEDESCRIPTORALLOCATOR->GetDSVDescriptorHeapStart());
}

void RenderTargetGroup::ClearRenderTargetView(uint32 index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorHeapStart(), index * _rtvHeapSize);
	CMD_LIST->ClearRenderTargetView(rtvHandle, _rtVec[index].clearColor, 0, nullptr);

	CMD_LIST->ClearDepthStencilView(SINGLEDESCRIPTORALLOCATOR->GetDSVDescriptorHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderTargetGroup::ClearRenderTargetView()
{
	for (uint32 i = 0; i < _rtCount; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SINGLEDESCRIPTORALLOCATOR->GetRTVDescriptorHeapStart(), i * _rtvHeapSize);
		CMD_LIST->ClearRenderTargetView(rtvHandle, _rtVec[i].clearColor, 0, nullptr);
	}
	CMD_LIST->ClearDepthStencilView(SINGLEDESCRIPTORALLOCATOR->GetDSVDescriptorHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}
