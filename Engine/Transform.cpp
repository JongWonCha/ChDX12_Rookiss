#include "pch.h"
#include "Engine.h"
#include "Transform.h"

Transform::Transform() : Component(COMPONENT_TYPE::TRANSFORM)
{
	_transform.offset = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
}

Transform::~Transform()
{
}

void Transform::Update()
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};

	CB_CONTAINER* AllocatedCB =  CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->Alloc();
	Constant_TransformMatrix* cb = (Constant_TransformMatrix*)AllocatedCB->pSystemMemAddr;
	*cb = _transform;
	DESCRIPTORPOOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);

	DEVICE->CopyDescriptorsSimple(1, cpuHandle, AllocatedCB->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Transform::SetTransform(const XMFLOAT4& transform)
{
	_transform.offset = transform;
}
