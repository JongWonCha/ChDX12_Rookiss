#pragma once
#include "Object.h"

class SingleDescriptorAllocator;

class Texture : public Object
{
public:
	Texture();
	virtual ~Texture();

	void Init(const wstring& path, const string &texId, shared_ptr<SingleDescriptorAllocator> singleDescAllocator);

	void Load(const wstring& path) override;

	//D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() { return _srvHandle; }

	void CreateTextureResource(const wstring& path, const string& texId);
	void CreateView(ComPtr<ID3D12Resource> texResource, ScratchImage* image, const string& texId);
	shared_ptr<TEXTURE_HANDLE> GetTextureHandle();

private:
	//ScratchImage					_image;
	//ComPtr<ID3D12Resource>			_tex2D;

	shared_ptr<SingleDescriptorAllocator> _singleDescAllocator;

	//unordered_map<string, TEXTURE_HANDLE*> stringTexMap;

	shared_ptr<TEXTURE_HANDLE>		_textureHandle = nullptr;

	//ComPtr<ID3D12DescriptorHeap>	_srvHeap;
	//D3D12_CPU_DESCRIPTOR_HANDLE		_srvHandle;
};

