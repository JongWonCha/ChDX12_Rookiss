#include "pch.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Engine.h"
#include "Transform.h"

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

	_material->PushData();

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};
	DESCRIPTORPOOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 0);
	auto before = gpuHandle.ptr;
	
	gpuHandle.ptr -= DESCRIPTORPOOL->GetSrvDescirptorSize() * 9; // TODO : offset �ϵ� �ڵ� �ذ�
	auto after = gpuHandle.ptr;
	
	_mesh->Render(gpuHandle);
}
