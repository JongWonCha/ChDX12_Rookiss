#include "pch.h"
#include "Texture.h"
#include "SingleDescriptorAllocator.h"
#include "Engine.h"

Texture::Texture() : Object(OBJECT_TYPE::TEXTURE)
{
}

Texture::~Texture()
{
	/*for (auto It = stringTexMap.begin(); It != stringTexMap.end(); ++It)
	{
		if (It->second != nullptr)
		{
			_singleDescAllocator->FreeDescriptorHandle(It->second->srv);
			delete It->second;
		}

	}
	stringTexMap.clear();*/
	if(_textureHandle->srvDesc.ptr != 0)
		SINGLEDESCRIPTORALLOCATOR->FreeDescriptorHandleForSRVUAV(_textureHandle->srvDesc);

	if (_textureHandle->rtvDesc.ptr != 0)
		SINGLEDESCRIPTORALLOCATOR->FreeDescriptorHandleForRTV(_textureHandle->rtvDesc);

	if (_textureHandle->dsvDesc.ptr != 0)
		SINGLEDESCRIPTORALLOCATOR->FreeDescriptorHandleForDSV(_textureHandle->dsvDesc);

	if(_textureHandle->uavDesc.ptr != 0)
		SINGLEDESCRIPTORALLOCATOR->FreeDescriptorHandleForSRVUAV(_textureHandle->uavDesc);

	_textureHandle = nullptr;

}

void Texture::Init(const wstring& path, const string& texId)
{
	//if (stringTexMap.find(texId) != stringTexMap.end()) // 이미 있다면
	//{
	//	return;
	//}
	//CreateTextureResource(path, texId);
}

void Texture::Load(const wstring& path)
{
	// TODO : PNG 파일 로드 시 잘못 로드되는 문제 해결
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

	_textureType = TEXTURE_TYPE::SRV;

	GEngine->GetGraphicsCmdQueue()->FlushResourceCommandQueue();

	CreateView(texResource, &image);
}

void Texture::Create(DXGI_FORMAT format, uint32 width, uint32 height,
	const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
	D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor)
{
	ComPtr<ID3D12Resource> texResource;

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
	desc.Flags = resFlags;

	D3D12_CLEAR_VALUE optimizedClearValue = {};
	D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr;
	D3D12_RESOURCE_STATES resourcesState = D3D12_RESOURCE_STATE_COMMON;

	if (resFlags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		resourcesState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
		pOptimizedClearValue = &optimizedClearValue;
	}
	else if (resFlags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	{
		resourcesState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
		float arrFloat[4] = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };
		optimizedClearValue = CD3DX12_CLEAR_VALUE(format, arrFloat);
		pOptimizedClearValue = &optimizedClearValue;
	}

	HRESULT hr = DEVICE->CreateCommittedResource(
		&heapProperty,
		heapFlags,
		&desc,
		resourcesState,
		pOptimizedClearValue,
		IID_PPV_ARGS(texResource.GetAddressOf())
	);

	assert(SUCCEEDED(hr));

	CreateFromResource(texResource);
}

//void Texture::CreateTextureResource(const wstring& path, const string& texId)
//{
//	ComPtr<ID3D12Resource> textureUploadHeap;
//	ComPtr<ID3D12Resource> texResource;
//	ScratchImage image;
//	wstring ext = fs::path(path).extension();
//
//	// 확장자에 따른 다른 메소드 수행
//	if (ext == L".dds" || ext == L".DDS")
//	{
//		::LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, image);
//	}
//	else if (ext == L".tga" || ext == L".TGA")
//	{
//		::LoadFromTGAFile(path.c_str(), nullptr, image);
//	}
//	else // png, jpg, jpeg, bmp
//	{
//		::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, image);
//	}
//
//	HRESULT hr = CreateTexture(DEVICE.Get(), image.GetMetadata(), &texResource);
//
//	if (FAILED(hr))
//		assert(nullptr);
//
//	vector<D3D12_SUBRESOURCE_DATA> subResources;
//
//	hr = DirectX::PrepareUpload(DEVICE.Get(),
//		image.GetImages(),
//		image.GetImageCount(),
//		image.GetMetadata(),
//		subResources);
//
//
//	const uint64 bufferSize = ::GetRequiredIntermediateSize(texResource.Get(), 0, static_cast<uint32>(subResources.size()));
//
//	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
//	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
//
//	hr = DEVICE->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(textureUploadHeap.GetAddressOf())
//	);
//
//	if (FAILED(hr))
//		assert(nullptr);
//
//	UpdateSubresources(RESOURCE_CMD_LIST.Get(),
//		texResource.Get(),
//		textureUploadHeap.Get(),
//		0, 0,
//		static_cast<unsigned int>(subResources.size()),
//		subResources.data());
//
//	GEngine->GetCmdQueue()->FlushResourceCommandQueue();
//
//	CreateView(texResource, &image, texId);
//}

void Texture::CreateFromResource(ComPtr<ID3D12Resource> tex2D)
{
	D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = {};
	D3D12_RESOURCE_DESC desc = tex2D->GetDesc();
	_textureHandle = make_shared<TEXTURE_HANDLE>();
	_textureHandle->pTexResource = tex2D.Get();
	_textureType = TEXTURE_TYPE::END;

	if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		if (SINGLEDESCRIPTORALLOCATOR->AllocDescriptorHandleForDSV(&descriptorHandle))
		{
			DEVICE->CreateDepthStencilView(tex2D.Get(), nullptr, descriptorHandle);
			_textureHandle->dsvDesc = descriptorHandle;
			_textureType = TEXTURE_TYPE::DSV;
		}
	}
	else
	{
		if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
		{
			BOOL hr = SINGLEDESCRIPTORALLOCATOR->AllocDescriptorHandleForRTV(&descriptorHandle);
			if (!hr) __debugbreak();

			DEVICE->CreateRenderTargetView(tex2D.Get(), nullptr, descriptorHandle);
			_textureHandle->rtvDesc = descriptorHandle;
			_textureType = TEXTURE_TYPE::RTV;
		}
		if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = desc.Format;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

			BOOL hr = SINGLEDESCRIPTORALLOCATOR->AllocDescriptorHandleForSRVUAV(&descriptorHandle);
			if (!hr) __debugbreak();
			DEVICE->CreateUnorderedAccessView(tex2D.Get(), nullptr, &uavDesc, descriptorHandle);
			_textureHandle->uavDesc = descriptorHandle;
			_textureType = TEXTURE_TYPE::UAV;
		}


		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		BOOL hr = SINGLEDESCRIPTORALLOCATOR->AllocDescriptorHandleForSRVUAV(&descriptorHandle);
		if (!hr) __debugbreak();

		DEVICE->CreateShaderResourceView(tex2D.Get(), &srvDesc, descriptorHandle);
		_textureHandle->srvDesc = descriptorHandle;

		if (_textureType == TEXTURE_TYPE::END) _textureType = TEXTURE_TYPE::SRV;
	}
	
}

void Texture::CreateView(ComPtr<ID3D12Resource> texResource, ScratchImage* image)
{
	D3D12_CPU_DESCRIPTOR_HANDLE srv = {};

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = image->GetMetadata().format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = image->GetMetadata().mipLevels;

	if (SINGLEDESCRIPTORALLOCATOR->AllocDescriptorHandleForSRVUAV(&srv))
	{
		DEVICE->CreateShaderResourceView(texResource.Get(), &srvDesc, srv);

		_textureHandle = make_shared<TEXTURE_HANDLE>();
		_textureHandle->pTexResource = texResource.Get();
		_textureHandle->srvDesc = srv;
		_textureType = TEXTURE_TYPE::SRV;
	}
}

shared_ptr<TEXTURE_HANDLE> Texture::GetTextureHandle()
{
	/*if (stringTexMap.find(texId) == stringTexMap.end())
	{
		__debugbreak();
		return nullptr;
	}

	return stringTexMap[texId];*/

	return _textureHandle;
}


D3D12_CPU_DESCRIPTOR_HANDLE* Texture::GetSRVHandle()
{
	if (_textureHandle == nullptr) return nullptr;
	return &_textureHandle->srvDesc;
}

D3D12_CPU_DESCRIPTOR_HANDLE* Texture::GetRTVHandle()
{
	if (_textureHandle == nullptr) return nullptr;
	return &_textureHandle->rtvDesc;
}

D3D12_CPU_DESCRIPTOR_HANDLE* Texture::GetDSVHandle()
{
	if (_textureHandle == nullptr) return nullptr;
	return &_textureHandle->dsvDesc;
}

D3D12_CPU_DESCRIPTOR_HANDLE* Texture::GetUAVHandle()
{
	if (_textureHandle == nullptr) return nullptr;
	return &_textureHandle->uavDesc;
}


