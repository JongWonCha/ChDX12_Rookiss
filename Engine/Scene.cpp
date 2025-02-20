#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Light.h"
#include "Resources.h"

void Scene::Awake()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Awake();
	}
}

void Scene::Start()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Start();
	}
}

void Scene::Update()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Update();
	}
}

void Scene::LateUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void Scene::FinalUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->FinalUpdate();
	}
}

void Scene::Render()
{
	PushLightData();

    int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	//int8 frontIndex = backIndex ^ 1;
    //backIndex ^= 1; // 비트 연산으로 0을 1로, 1을 0으로 변환
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetViews(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT, 0, 0);

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->ClearRenderTargetViews(RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT, RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT, 0);

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT, 0, 0);
	shared_ptr<Camera> mainCamera = _cameras[0];
	mainCamera->SortGameObject();
	mainCamera->Render_Deferred();

	RenderLights();
	RenderFinal();

	mainCamera->Render_Forward();

	for (auto& camera : _cameras)
	{
		if (camera == mainCamera) continue;

		camera->SortGameObject();
		camera->Render_Forward();
	}
}

void Scene::RenderLights()
{
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->OMSetRenderTargets(RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT, RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT, 0);

	for (auto& light : _lights)
	{
		light->Render();
	}
}

void Scene::RenderFinal()
{

	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTarget(1, backIndex);

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};
	DESCRIPTORPOOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);

	shared_ptr<Material> finalMaterial = GET_SINGLE(Resources)->Get<Material>(L"Final");
	finalMaterial->PushData();

	//GET_SINGLE(Resources)->Get<Material>(L"Final")->PushData();
	DESCRIPTORPOOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 0);
	gpuHandle.ptr -= DESCRIPTORPOOL->GetSrvDescirptorSize() * 8;
	GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle")->Render(gpuHandle);
}

void Scene::PushLightData()
{
	Constant_LightParams lightParams = {};
	for (auto& light : _lights)
	{
		if (light == nullptr)
			continue;

		const LightInfo& lightInfo = light->GetLightInfo();

		light->SetLightIndex(lightParams.lightCount);

		lightParams.lights[lightParams.lightCount] = lightInfo;
		lightParams.lightCount++;
	}
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::LIGHT)->SetGlobalData(&lightParams, sizeof(lightParams));
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{
	if (gameObject->GetCamera() != nullptr)
	{
		_cameras.emplace_back(gameObject->GetCamera());
	}
	else if (gameObject->GetLight() != nullptr)
	{
		_lights.emplace_back(gameObject->GetLight());
	}

	_gameObjects.emplace_back(gameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	if (gameObject->GetCamera() != nullptr)
	{
		auto findIt = std::find(_cameras.begin(), _cameras.end(), gameObject->GetCamera());
		if (findIt != _cameras.end())
			_cameras.erase(findIt);
	}
	else if (gameObject->GetLight() != nullptr)
	{
		auto findIt = std::find(_lights.begin(), _lights.end(), gameObject->GetLight());
		if (findIt != _lights.end())
			_lights.erase(findIt);
	}

	auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
	if (findIt != _gameObjects.end())
	{
		_gameObjects.erase(findIt);
	}
}
