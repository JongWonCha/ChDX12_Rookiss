#pragma once
#include "Object.h"

class SingleDescriptorAllocator;

enum class TEXTURE_TYPE : uint8
{
	SRV,
	UAV,
	RTV,
	DSV,
	END,
};

class Texture : public Object
{
public:
	Texture();
	virtual ~Texture();

	void Init(const wstring& path, const string &texId);

	void Load(const wstring& path) override;

	void Create(DXGI_FORMAT format, uint32 width, uint32 height,
		const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
		D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor = Vec4());

	//D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() { return _srvHandle; }

	void CreateTextureResource(const wstring& path, const string& texId);
	void CreateFromResource(ComPtr<ID3D12Resource> tex2D);
	void CreateView(ComPtr<ID3D12Resource> texResource, ScratchImage* image);
	shared_ptr<TEXTURE_HANDLE> GetTextureHandle();

	D3D12_CPU_DESCRIPTOR_HANDLE* GetSRVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE* GetRTVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE* GetDSVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE* GetUAVHandle();

private:
	TEXTURE_TYPE _textureType = TEXTURE_TYPE::SRV;

	shared_ptr<TEXTURE_HANDLE>		_textureHandle = nullptr;
};

