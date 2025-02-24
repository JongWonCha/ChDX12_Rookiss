#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Transform.h"
#include "Light.h"
#include "ParticleSystem.h"

#include "TestCameraScript.h"
#include "Resources.h"

//shared_ptr<Texture> texture = make_shared<Texture>();

void SceneManager::Update()
{
	if (_activeScene == nullptr)
		return;

	_activeScene->Update();
	_activeScene->LateUpdate();
	_activeScene->FinalUpdate();
}

void SceneManager::Render()
{
	if (_activeScene)
		_activeScene->Render();
}

void SceneManager::LoadScene(wstring sceneName)
{
	// TODO : 기존 Scene 정리
	// TODO : 파일에서 Scene 정보 로드

	_activeScene = LoadTestScene();

	_activeScene->Awake();
	_activeScene->Start();
}

void SceneManager::SetLayerName(uint8 index, const wstring& name)
{
	const wstring& prevName = _layerNames[index];
	_layerIndex.erase(prevName);

	_layerNames[index] = name;
	_layerIndex[name] = index;
}

uint8 SceneManager::LayerNameToIndex(const wstring& name)
{
	auto findIt = _layerIndex.find(name);
	if (findIt == _layerIndex.end())
		return 0;

	return findIt->second;
}

shared_ptr<Scene> SceneManager::LoadTestScene()
{
	shared_ptr<Scene> scene = make_shared<Scene>();

	SetLayerName(0, L"Default");
	SetLayerName(1, L"UI");
	
	/*shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
	shared_ptr<Mesh> cubeMesh = GET_SINGLE(Resources)->LoadCubeMesh();*/
#pragma region COMPUTESHADER
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// Create UAV Texture 생성
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		material->PushComputeData();
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};

		COMPUTE_DESC_POOL->AllocDescriptorTable(&cpuHandle, &gpuHandle, 1);
		DEVICE->CopyDescriptorsSimple(1, cpuHandle, *texture->GetUAVHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>());
		camera->AddComponent(make_shared<TestCameraScript>());
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI안찍음.
		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>());
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll();
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음.
		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region SKYBOX
	{
		shared_ptr<GameObject> skybox = make_shared<GameObject>();
		skybox->AddComponent(make_shared<Transform>());
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Sky01", L"..\\Resources\\Texture\\skySphere.jpg");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		skybox->AddComponent(meshRenderer);
		scene->AddGameObject(skybox);
	}
#pragma endregion

#pragma region UI_Test
	for(int32 i = 0; i < 6; ++i)
	{
		shared_ptr<GameObject> rect = make_shared<GameObject>();
		rect->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		rect->AddComponent(make_shared<Transform>());
		rect->GetTransform()->SetLocalPosition(Vec3(-520.f + (i * 140), 350.f, 500.f));
		rect->GetTransform()->SetLocalScale(Vec3(120.f, 80.f, 100.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> rectangleMesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(rectangleMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");
			shared_ptr<Texture> texture;
			if (i < 3)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
			else if (i < 5)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
			else
				texture = GET_SINGLE(Resources)->Get<Texture>(L"UAVTexture");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		rect->AddComponent(meshRenderer);
		scene->AddGameObject(rect);
	}
#pragma endregion

#pragma region Cube
	{
		shared_ptr<GameObject> sphere = make_shared<GameObject>();
		
		sphere->AddComponent(make_shared<Transform>());
		sphere->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		sphere->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 200.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Deferred");
			shared_ptr<Texture> base = GET_SINGLE(Resources)->Load<Texture>(L"base", L"..\\Resources\\Texture\\cliff_base.dds");
			shared_ptr<Texture> normal = GET_SINGLE(Resources)->Load<Texture>(L"normal", L"..\\Resources\\Texture\\cliff_normal.dds");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, base);
			material->SetTexture(1, normal);
			meshRenderer->SetMaterial(material);
		}
		sphere->AddComponent(meshRenderer);
		scene->AddGameObject(sphere);
	}
#pragma endregion

//#pragma region Cube
//	{
//		shared_ptr<GameObject> sphere = make_shared<GameObject>();
//		sphere->AddComponent(make_shared<Transform>());
//		sphere->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
//		sphere->GetTransform()->SetLocalPosition(Vec3(100.f, 100.f, 200.f));
//		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
//		meshRenderer->SetMesh(cubeMesh);
//
//		{
//			shared_ptr<Shader> shader = make_shared<Shader>();
//			shader->Init(L"..\\Resources\\Shader\\default.hlsl");
//			shared_ptr<Material> material = make_shared<Material>();
//			material->SetShader(shader);
//			material->SetTexture(0, texture->GetTextureHandle("park"));
//			meshRenderer->SetMaterial(material);
//		}
//		sphere->AddComponent(meshRenderer);
//		scene->AddGameObject(sphere);
//	}
//#pragma endregion

#pragma region Directional Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		//light->GetTransform()->SetLocalPosition(Vec3(0.f, 150.f, 150.f));
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightDirection(Vec3(0, 0, 1.f));
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(1.f, 0.f, 0.f));
		light->GetLight()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
		light->GetLight()->SetSpecular(Vec3(0.2f, 0.2f, 0.2f));

		scene->AddGameObject(light);
	}
#pragma endregion

#pragma region Point Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0.f, 100.f, 150.f));
		light->AddComponent(make_shared<Light>());
		//light->GetLight()->SetLightDirection(Vec3(-1.f, -1.f, 0));
		light->GetLight()->SetLightType(LIGHT_TYPE::POINT_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(0.0f, 0.5f, 0.0f));
		light->GetLight()->SetAmbient(Vec3(0.0f, 0.3f, 0.0f));
		light->GetLight()->SetSpecular(Vec3(0.0f, 0.3f, 0.0f));
		light->GetLight()->SetLightRange(200.f);

		scene->AddGameObject(light);
	}
#pragma endregion

#pragma region Spot Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(75.f, 0.f, 150.f));
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightDirection(Vec3(-1.f, 0, 0));
		light->GetLight()->SetLightType(LIGHT_TYPE::SPOT_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(0.0f, 0.f, 0.5f));
		light->GetLight()->SetAmbient(Vec3(0.0f, 0.0f, 0.1f));
		light->GetLight()->SetSpecular(Vec3(0.0f, 0.0f, 0.1f));
		light->GetLight()->SetLightRange(200.f);
		light->GetLight()->SetLightAngle(3.14f / 2);

		scene->AddGameObject(light);
	}
#pragma endregion

#pragma region ParticleSystem
	{
		shared_ptr<GameObject> particle = make_shared<GameObject>();
		particle->AddComponent(make_shared<Transform>());
		particle->AddComponent(make_shared<ParticleSystem>());
		particle->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 100.f));
		scene->AddGameObject(particle);
	}
#pragma endregion


	return scene;
}
