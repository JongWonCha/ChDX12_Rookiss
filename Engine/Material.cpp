#include "pch.h"
#include "Material.h"
#include "Engine.h"
#include "ConstantBuffer.h"

Material::Material() : Object(OBJECT_TYPE::MATERIAL)
{
	for (auto It = _textures.begin(); It != _textures.end(); ++It)
		*It = nullptr;

	_params.SetAllFloat(0.f);
	_params.SetAllInt(0);
	_params.SetAllVec2(Vec2(0.f, 0.f));
}

Material::~Material()
{
}

void Material::PushGraphicsData()
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};

	CB_CONTAINER* allocatedCB = CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->Alloc();
	Constant_MaterialParams* sysMem = (Constant_MaterialParams*)allocatedCB->pSystemMemAddr;
	*sysMem = _params;

	GRAPHICS_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);

	//CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDesc(cpuHandle, 0, DESCRIPTORPOOL->GetSrvDescirptorSize());
	DEVICE->CopyDescriptorsSimple(1, cpuHandle, allocatedCB->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	GRAPHICS_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 2);

	for (int i = 0; i < MATERIAL_TEXTURE_COUNT; ++i)
	{
		GRAPHICS_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);
		if(_textures[i])
			DEVICE->CopyDescriptorsSimple(1, cpuHandle, _textures[i]->GetTextureHandle()->srvDesc, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	_shader->Update();
}

void Material::PushComputeData()
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};

	CB_CONTAINER* allocatedCB = CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->Alloc();
	Constant_MaterialParams* systemMem = (Constant_MaterialParams*)allocatedCB->pSystemMemAddr;
	*systemMem = _params;
	COMPUTE_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 2); //b0, b1
	COMPUTE_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);	// b2
	DEVICE->CopyDescriptorsSimple(1, cpuHandle, allocatedCB->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	COMPUTE_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 2);
	for (int i = 0; i < MATERIAL_TEXTURE_COUNT; ++i)
	{
		COMPUTE_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);
		if (_textures[i])
			DEVICE->CopyDescriptorsSimple(1, cpuHandle, _textures[i]->GetTextureHandle()->srvDesc, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	COMPUTE_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 6);
}

void Material::Dispatch(uint32 x, uint32 y, uint32 z)
{
	_shader->Update();

	COMPUTE_CMD_LIST->SetDescriptorHeaps(1, COMPUTE_DESC_POOL->GetDescriptorHeap().GetAddressOf());

	COMPUTE_CMD_LIST->SetComputeRootDescriptorTable(0, COMPUTE_DESC_POOL->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	COMPUTE_CMD_LIST->Dispatch(x, y, z);

	GEngine->GetComputeCmdQueue()->FlushComputeCommandQueue();
}


