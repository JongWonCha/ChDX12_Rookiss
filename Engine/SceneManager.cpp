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
	
	shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
	shared_ptr<Mesh> cubeMesh = GET_SINGLE(Resources)->LoadCubeMesh();
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
	for(int32 i = 0; i < 3; ++i)
	{
		shared_ptr<GameObject> rect = make_shared<GameObject>();
		rect->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		rect->AddComponent(make_shared<Transform>());
		rect->GetTransform()->SetLocalPosition(Vec3(-500.f + (i * 160), 300.f, 500.f));
		rect->GetTransform()->SetLocalScale(Vec3(150.f, 100.f, 100.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Forward");
			//shared_ptr<Texture> base = GET_SINGLE(Resources)->Load<Texture>(L"normalFlat", L"..\\Resources\\Texture\\normal_flat.dds");
			shared_ptr<Texture> texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
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
		shared_ptr<Texture> base = GET_SINGLE(Resources)->Load<Texture>(L"base", L"..\\Resources\\Texture\\cliff_base.dds");
		shared_ptr<Texture> normal = GET_SINGLE(Resources)->Load<Texture>(L"normal", L"..\\Resources\\Texture\\cliff_normal.dds");
		sphere->AddComponent(make_shared<Transform>());
		sphere->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		sphere->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 150.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		meshRenderer->SetMesh(cubeMesh);
		
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Deferred");
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

#pragma region Green Directional Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		//light->GetTransform()->SetLocalPosition(Vec3(0.f, 150.f, 150.f));
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightDirection(Vec3(1.f, 0.f, 1.f));
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(0.5f, 0.5f, 0.5f));
		light->GetLight()->SetAmbient(Vec3(0.f, 0.f, 0.f));
		light->GetLight()->SetSpecular(Vec3(0.f, 0.f, 0.f));

		scene->AddGameObject(light);
	}

#pragma endregion

//#pragma region Red Point Light
//	{
//		shared_ptr<GameObject> light = make_shared<GameObject>();
//		light->AddComponent(make_shared<Transform>());
//		light->GetTransform()->SetLocalPosition(Vec3(150.f, 150.f, 150.f));
//		light->AddComponent(make_shared<Light>());
//		//light->GetLight()->SetLightDirection(Vec3(0.f, -1.f, 0.f));
//		light->GetLight()->SetLightType(LIGHT_TYPE::POINT_LIGHT);
//		light->GetLight()->SetDiffuse(Vec3(1.f, 0.1f, 0.1f));
//		light->GetLight()->SetAmbient(Vec3(0.1f, 0.f, 0.f));
//		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
//		light->GetLight()->SetLightRange(10000.f);
//		//light->GetLight()->SetLightAngle(XM_PI / 4);
//		scene->AddGameObject(light);
//	}
//#pragma endregion
//
//#pragma region Blue Spot Light
//	{
//		shared_ptr<GameObject> light = make_shared<GameObject>();
//		light->AddComponent(make_shared<Transform>());
//		light->GetTransform()->SetLocalPosition(Vec3(-150.f, 0.f, 150.f));
//		light->AddComponent(make_shared<Light>());
//		light->GetLight()->SetLightDirection(Vec3(1.f, 0.f, 0.f));
//		light->GetLight()->SetLightType(LIGHT_TYPE::SPOT_LIGHT);
//		light->GetLight()->SetDiffuse(Vec3(0.f, 0.1f, 1.f));
//		//light->GetLight()->SetAmbient(Vec3(0.f, 0.f, 0.1f));
//		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
//		light->GetLight()->SetLightRange(10000.f);
//		light->GetLight()->SetLightAngle(XM_PI / 4);
//		scene->AddGameObject(light);
//	}
//#pragma endregion



	return scene;
}
