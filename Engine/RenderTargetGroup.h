#pragma once
#include "Texture.h"

enum class RENDER_TARGET_GROUP_TYPE : uint8
{
	SWAP_CHAIN,	// back buffer, front buffer
	G_BUFFER,	// position, normal, color
	LIGHTING,	// diffuse light, specular light
	DEPTH_STENCIL,	// depth, stencil
	END,
};

enum
{
	RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT = 3,
	RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT = 2,
	RENDER_TARGET_GROUP_COUNT = static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::END),
};

struct RenderTarget
{
	shared_ptr<Texture> target;
	float clearColor[4];
};

class RenderTargetGroup
{
public:
	void Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec);

	void OMSetRenderTarget(uint32 count, uint32 offset);	// for RTV
	void OMSetRenderTargets(uint32 count, uint32 offset, uint32 index);	// for GBuffer	

	void ClearRenderTargetView(uint32 index);
	void ClearRenderTargetViews(uint32 count, uint32 offset, uint32 index);

	shared_ptr<Texture>& GetRTTexture(uint32 index) { return _rtVec[index].target; }
	void ClearRTVector() { _rtVec.clear(); }
	shared_ptr<Texture>& GetDSTexture() { return _dsTexture; }

	void WaitTargetToResource();
	void WaitResourceToTarget();

private:
	RENDER_TARGET_GROUP_TYPE		_groupType;
	vector<RenderTarget>			_rtVec;
	uint32							_rtCount;
	shared_ptr<Texture>				_dsTexture;	
	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;

private:
	uint32						_rtvHeapSize;
	D3D12_CPU_DESCRIPTOR_HANDLE _rtvHeapBegin;
	D3D12_CPU_DESCRIPTOR_HANDLE _dsvHeapBegin;

private:
	D3D12_RESOURCE_BARRIER	_targetToResource[8];
	D3D12_RESOURCE_BARRIER	_resourceToTarget[8];
};

