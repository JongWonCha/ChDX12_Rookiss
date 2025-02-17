#pragma once
#include "Object.h"

class SingleDescriptorAllocator;

enum class TEXTURE_TYPE : uint8
{
	SRV_UAV,
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
	void CreateView(ComPtr<ID3D12Resource> texResource, ScratchImage* image, const string& texId);
	shared_ptr<TEXTURE_HANDLE> GetTextureHandle();

private:
	TEXTURE_TYPE _textureType = TEXTURE_TYPE::SRV_UAV;
	//ScratchImage					_image;
	//ComPtr<ID3D12Resource>			_tex2D;

	//shared_ptr<SingleDescriptorAllocator> _singleDescAllocator;

	//unordered_map<string, TEXTURE_HANDLE*> stringTexMap;

	shared_ptr<TEXTURE_HANDLE>		_textureHandle = nullptr;

	//ComPtr<ID3D12DescriptorHeap>	_srvHeap;
	//D3D12_CPU_DESCRIPTOR_HANDLE		_srvHandle;
};

