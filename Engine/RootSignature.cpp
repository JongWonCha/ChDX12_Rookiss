#include "pch.h"
#include "RootSignature.h"
#include "Engine.h"

void RootSignature::Init()
{
	CreateSamplerDesc();
	CreateGraphicsRootSignature();
	CreateComputeRootSignature();
}

void RootSignature::CreateSamplerDesc()
{
	_samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);
}

void RootSignature::CreateGraphicsRootSignature()
{
	// 루트 시그니처 넣을 수 있는 종류
	// constant, descriptor, table
	CD3DX12_DESCRIPTOR_RANGE range[2] = {};
	range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 4, 1); // b1 ~ b4
	range[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 0); // t0 ~ t4

	CD3DX12_ROOT_PARAMETER param[2] = {};
	param[0].InitAsConstantBufferView(static_cast<uint32>(0)); // b0
	param[1].InitAsDescriptorTable(_countof(range), range, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_ROOT_SIGNATURE_DESC sigDesc;
	sigDesc.Init(_countof(param), param, 1, &_samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;
	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	DEVICE->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_graphicsRootSignature));
}

void RootSignature::CreateComputeRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE range[3] = {};
	range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 5, 0);	// b0 ~ b4
	range[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 10, 0);	// t0 ~ t9
	range[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 5, 0);	// u0 ~ u4

	CD3DX12_ROOT_PARAMETER param[1] = {};
	param[0].InitAsDescriptorTable(_countof(range), range);

	CD3DX12_ROOT_SIGNATURE_DESC sigDesc;
	sigDesc.Init(_countof(param), param);
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;
	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	DEVICE->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_computeRootSignature));

	COMPUTE_CMD_LIST->SetComputeRootSignature(_computeRootSignature.Get());
}
