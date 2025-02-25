#include "pch.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Engine.h"
#include "Transform.h"
#include "InstancingBuffer.h"

MeshRenderer::MeshRenderer() : Component(COMPONENT_TYPE::MESHRENDERER)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Update()
{
}

void MeshRenderer::Render()
{
	GetTransform()->PushData();

	_material->PushGraphicsData();

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};
	GRAPHICS_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 6);
	GRAPHICS_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 0);
	
	gpuHandle.ptr -= GRAPHICS_DESC_POOL->GetSrvDescirptorSize() * 14; // TODO : offset 하드 코딩 해결
	
	_mesh->Render(gpuHandle);
}

void MeshRenderer::Render(shared_ptr<InstancingBuffer>& buffer)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE start = {};
	GRAPHICS_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);
	start = gpuHandle;

	buffer->PushData();
	_material->PushGraphicsData();
	GRAPHICS_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 6);
	_mesh->Render(start, buffer);
}

uint64 MeshRenderer::GetInstanceID()
{
	if (_mesh == nullptr || _material == nullptr) return 0;

	InstanceID instanceID{ _mesh->GetID(), _material->GetID() };
	return instanceID.id;
}
