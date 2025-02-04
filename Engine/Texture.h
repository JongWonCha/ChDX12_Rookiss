#pragma once
class SingleDescriptorAllocator;

class Texture
{
public:
	void Init(const wstring& path, const string &texId, shared_ptr<SingleDescriptorAllocator> singleDescAllocator);

	//D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() { return _srvHandle; }

	void CreateTextureResource(const wstring& path, const string& texId);
	void CreateView(ComPtr<ID3D12Resource> texResource, ScratchImage* image, const string& texId);
	TEXTURE_HANDLE * GetTextureHandle(const string& texId);

	~Texture();
private:
	//ScratchImage					_image;
	//ComPtr<ID3D12Resource>			_tex2D;

	shared_ptr<SingleDescriptorAllocator> _singleDescAllocator;

	unordered_map<string, TEXTURE_HANDLE*> stringTexMap;

	//ComPtr<ID3D12DescriptorHeap>	_srvHeap;
	//D3D12_CPU_DESCRIPTOR_HANDLE		_srvHandle;
};

