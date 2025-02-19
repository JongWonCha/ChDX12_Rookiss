#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Light.h"

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
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetViews(backIndex);

	for (auto& gameObject : _gameObjects)
	{
		if (gameObject->GetCamera() == nullptr) continue;

		//gameObject->GetCamera()->Render();

		gameObject->GetCamera()->SortGameObject();

		// Deferred OMSet
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets(backIndex);
		gameObject->GetCamera()->Render_Deferred();

		// TODO : Light OMSet

		// Swapchain OMSet
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTarget(1, backIndex);
		gameObject->GetCamera()->Render_Forward();
	}
}

void Scene::PushLightData()
{
	/*CB_CONTAINER* lightCB = CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::LIGHT)->Alloc();
	Constant_LightParams* lightParams = (Constant_LightParams*)lightCB->pSystemMemAddr;

	for (auto& gameObject : _gameObjects)
	{
		if (gameObject->GetLight() == nullptr) continue;

		const LightInfo& lightInfo = gameObject->GetLight()->GetLightInfo();

		lightParams->lights[lightParams->lightCount] = lightInfo;
		lightParams->lightCount++;
	}
	CMD_LIST->SetGraphicsRootConstantBufferView(0, lightCB->pGPUMemAddr);*/

	Constant_LightParams lightParams = {};
	for (auto& gameObject : _gameObjects)
	{
		if (gameObject->GetLight() == nullptr)
			continue;

		const LightInfo& lightInfo = gameObject->GetLight()->GetLightInfo();

		lightParams.lights[lightParams.lightCount] = lightInfo;
		lightParams.lightCount++;
	}
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::LIGHT)->SetGlobalData(&lightParams, sizeof(lightParams));
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{
	_gameObjects.emplace_back(gameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
	if (findIt != _gameObjects.end())
	{
		_gameObjects.erase(findIt);
	}
}
