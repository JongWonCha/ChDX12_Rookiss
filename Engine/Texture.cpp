#include "pch.h"
#include "Texture.h"
#include "SingleDescriptorAllocator.h"
#include "Engine.h"

void Texture::Init(const wstring& path, const string &texId, shared_ptr<SingleDescriptorAllocator> singleDescAllocator)
{
	if (stringTexMap.find(texId) != stringTexMap.end()) // 이미 있다면
	{
		return;
	}
	_singleDescAllocator = singleDescAllocator;
	CreateTextureResource(path, texId);
}

void Texture::CreateTextureResource(const wstring& path, const string& texId)
{
	ComPtr<ID3D12Resource> textureUploadHeap;
	ComPtr<ID3D12Resource> texResource;
	ScratchImage image;
	wstring ext = fs::path(path).extension();

	// 확장자에 따른 다른 메소드 수행
	if (ext == L".dds" || ext == L".DDS")
	{
		::LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, image);
	}
	else if (ext == L".tga" || ext == L".TGA")
	{
		::LoadFromTGAFile(path.c_str(), nullptr, image);
	}
	else // png, jpg, jpeg, bmp
	{
		::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, image);
	}

	HRESULT hr = CreateTexture(DEVICE.Get(), image.GetMetadata(), &texResource);

	if (FAILED(hr))
		assert(nullptr);

	vector<D3D12_SUBRESOURCE_DATA> subResources;

	hr = DirectX::PrepareUpload(DEVICE.Get(),
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		subResources);


	const uint64 bufferSize = ::GetRequiredIntermediateSize(texResource.Get(), 0, static_cast<uint32>(subResources.size()));

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	
	hr = DEVICE->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(textureUploadHeap.GetAddressOf())
	);

	if (FAILED(hr))
		assert(nullptr);

	UpdateSubresources(RESOURCE_CMD_LIST.Get(),
		texResource.Get(),
		textureUploadHeap.Get(),
		0, 0,
		static_cast<unsigned int>(subResources.size()),
		subResources.data());

	GEngine->GetCmdQueue()->FlushResourceCommandQueue();

	CreateView(texResource, &image, texId);
}

void Texture::CreateView(ComPtr<ID3D12Resource> texResource, ScratchImage* image, const string& texId)
{
	D3D12_CPU_DESCRIPTOR_HANDLE srv = {};

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = image->GetMetadata().format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = image->GetMetadata().mipLevels;

	if (_singleDescAllocator->AllocDescriptorHandle(&srv))
	{
		DEVICE->CreateShaderResourceView(texResource.Get(), &srvDesc, srv);

		TEXTURE_HANDLE* tempHandle = new TEXTURE_HANDLE;
		tempHandle->pTexResource = texResource.Get();
		tempHandle->srv = srv;
		stringTexMap[texId] = tempHandle;
	}
}

TEXTURE_HANDLE* Texture::GetTextureHandle(const string& texId)
{
	if (stringTexMap.find(texId) == stringTexMap.end())
	{
		__debugbreak();
		return nullptr;
	}

	return stringTexMap[texId];
}

Texture::~Texture()
{
	for (auto It = stringTexMap.begin(); It != stringTexMap.end(); ++It)
	{
		if (It->second != nullptr)
		{
			_singleDescAllocator->FreeDescriptorHandle(It->second->srv);
			delete It->second;
		}
			
	}
	stringTexMap.clear();
}
