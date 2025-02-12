#include "pch.h"
#include "Material.h"
#include "Engine.h"
#include "ConstantBuffer.h"

void Material::PushData()
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};

	CB_CONTAINER* allocatedCB = CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->Alloc();
	Constant_MaterialParams* sysMem = (Constant_MaterialParams*)allocatedCB->pSystemMemAddr;
	*sysMem = _params;

	DESCRIPTORPOOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);

	//CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDesc(cpuHandle, 0, DESCRIPTORPOOL->GetSrvDescirptorSize());
	DEVICE->CopyDescriptorsSimple(1, cpuHandle, allocatedCB->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (int i = 0; i < MATERIAL_TEXTURE_COUNT; ++i)
	{
		DESCRIPTORPOOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);
		if(_textures[i])
			DEVICE->CopyDescriptorsSimple(1, cpuHandle, _textures[i]->srv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	_shader->Update();
}

Material::Material()
{
	for (auto It = _textures.begin(); It != _textures.end(); ++It)
		*It = nullptr;

	_params.SetAllFloat(0.f);
	_params.SetAllInt(0);
}
