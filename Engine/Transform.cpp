#include "pch.h"
#include "Engine.h"
#include "Transform.h"
#include "Camera.h"

Transform::Transform() : Component(COMPONENT_TYPE::TRANSFORM)
{
	
}

Transform::~Transform()
{
}

void Transform::Update()
{
	/*D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};

	CB_CONTAINER* AllocatedCB =  CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->Alloc();
	Constant_TransformParams* cb = (Constant_TransformParams*)AllocatedCB->pSystemMemAddr;
	*cb = _transform;
	DESCRIPTORPOOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);

	DEVICE->CopyDescriptorsSimple(1, cpuHandle, AllocatedCB->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);*/
}

void Transform::FinalUpdate()
{
	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	_matLocal = matRotation * matScale * matTranslation;
	_matWorld = _matLocal;

	shared_ptr<Transform> parent = GetParent().lock();
	if (parent != nullptr)
	{
		_matWorld *= parent->GetLocalToWorldMatrix();
	}
}

void Transform::PushData()
{
	Matrix matWVP = _matWorld * Camera::S_MatView * Camera::S_MatProjection;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};

	CB_CONTAINER* AllocatedCB = CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->Alloc();
	Constant_TransformParams* cb = (Constant_TransformParams*)AllocatedCB->pSystemMemAddr;
	cb->matWVP = matWVP;
	DESCRIPTORPOOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);

	DEVICE->CopyDescriptorsSimple(1, cpuHandle, AllocatedCB->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
