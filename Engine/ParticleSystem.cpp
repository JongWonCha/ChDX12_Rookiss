#include "pch.h"  
#include "ParticleSystem.h"  
#include "StructuredBuffer.h"  
#include "Resources.h"  
#include "Engine.h"  
#include "Timer.h"
#include "Transform.h"

ParticleSystem::ParticleSystem() : Component(COMPONENT_TYPE::PARTICLE_SYSTEM)  
{  
	_particleBuffer = make_shared<StructuredBuffer>();  
	_particleBuffer->Init(sizeof(ParticleInfo), _maxParticle);  
	
	_computeSharedBuffer = make_shared<StructuredBuffer>();  
	_computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1);  
	
	_mesh = GET_SINGLE(Resources)->LoadPointMesh();  
	_material = GET_SINGLE(Resources)->Get<Material>(L"Particle");  
	shared_ptr<Texture> tex = GET_SINGLE(Resources)->Load<Texture>(  
		L"Bubbles", L"..\\Resources\\Texture\\Particle\\bubble.png");  
	
	_material->SetTexture(0, tex);  
	
	_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle");  
}  

ParticleSystem::~ParticleSystem()  
{  
}  

void ParticleSystem::FinalUpdate()  
{  
	//COMPUTE_DESC_POOL->Reset();
	_accTime += DELTATIME;

	int32 add = 0;
	if (_createInterval < _accTime)
	{
		_accTime = _accTime - _createInterval;
		add = 1;
	}

	// b0 ~ b4, t0 ~ t9, u0 ~ u4
	_particleBuffer->PushComputeUAVData(15); 
	_computeSharedBuffer->PushComputeUAVData(16); 

	_computeMaterial->SetInt(0, _maxParticle);
	_computeMaterial->SetInt(1, add);

	_computeMaterial->SetVec2(1, Vec2(DELTATIME, _accTime));
	_computeMaterial->SetVec4(0, Vec4(_minLifeTime, _maxLifeTime, _minSpeed, _maxSpeed));
	_computeMaterial->PushComputeData();

	_computeMaterial->Dispatch(1, 1, 1);
}  

void ParticleSystem::Render()  
{  
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE start = {};
	GRAPHICS_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 0);
	start = gpuHandle;

	GetTransform()->PushData();
	_material->SetFloat(0, _startScale);
	_material->SetFloat(1, _endScale);
	_material->PushGraphicsData();
	GRAPHICS_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 5);
	_particleBuffer->PushGraphicsData();
	GRAPHICS_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);

	_mesh->Render(start, _maxParticle);
}
