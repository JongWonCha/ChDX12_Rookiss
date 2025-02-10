#include "pch.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Engine.h"

MeshRenderer::MeshRenderer() : Component(COMPONENT_TYPE::MESHRENDERER)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Render()
{
	//GetTransform()->Update();

	_material->Update();

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};
	DESCRIPTORPOOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 0);
	gpuHandle.ptr -= DESCRIPTORPOOL->GetSrvDescirptorSize() * 7;

	_mesh->Render(gpuHandle);
}
