#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Transform.h"

#include "TestCameraScript.h"
#include "Resources.h"

shared_ptr<Texture> texture = make_shared<Texture>();

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
	if (_activeScene == nullptr) return;

	const vector<shared_ptr<GameObject>>& gameObjects = _activeScene->GetGameObjects();
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetCamera() == nullptr) continue;

		gameObject->GetCamera()->Render();
	}
}

void SceneManager::LoadScene(wstring sceneName)
{
	// TODO : 기존 Scene 정리
	// TODO : 파일에서 Scene 정보 로드

	_activeScene = LoadTestScene();

	_activeScene->Awake();
	_activeScene->Start();
}

shared_ptr<Scene> SceneManager::LoadTestScene()
{
	shared_ptr<Scene> scene = make_shared<Scene>();
	texture->Init(L"..\\Resources\\Texture\\veigar.jpg", "veigar", GEngine->GetSingleDescriptorAllocator());
	texture->Init(L"..\\Resources\\Texture\\parktabon.png", "park", GEngine->GetSingleDescriptorAllocator());
	shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
	shared_ptr<Mesh> cubeMesh = GET_SINGLE(Resources)->LoadCubeMesh();
#pragma region Camera
	shared_ptr<GameObject> camera = make_shared<GameObject>();
	camera->AddComponent(make_shared<Transform>());
	camera->AddComponent(make_shared<Camera>());
	camera->AddComponent(make_shared<TestCameraScript>());
	camera->GetTransform()->SetLocalPosition(Vec3(0.f, 100.f, 0.f));
	scene->AddGameObject(camera);
#pragma endregion

#pragma region Sphere
	{
		shared_ptr<GameObject> sphere = make_shared<GameObject>();
		sphere->AddComponent(make_shared<Transform>());
		sphere->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		sphere->GetTransform()->SetLocalPosition(Vec3(-100.f, 100.f, 200.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		meshRenderer->SetMesh(sphereMesh);
		
		{
			shared_ptr<Shader> shader = make_shared<Shader>();
			shader->Init(L"..\\Resources\\Shader\\default.hlsl");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture->GetTextureHandle("veigar"));
			meshRenderer->SetMaterial(material);
		}
		sphere->AddComponent(meshRenderer);
		scene->AddGameObject(sphere);
	}
#pragma endregion

#pragma region Cube
	{
		shared_ptr<GameObject> sphere = make_shared<GameObject>();
		sphere->AddComponent(make_shared<Transform>());
		sphere->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		sphere->GetTransform()->SetLocalPosition(Vec3(100.f, 100.f, 200.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		meshRenderer->SetMesh(cubeMesh);

		{
			shared_ptr<Shader> shader = make_shared<Shader>();
			shader->Init(L"..\\Resources\\Shader\\default.hlsl");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture->GetTextureHandle("park"));
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
//		sphere->GetTransform()->SetLocalPosition(Vec3(150.f, 100.f, 200.f));
//		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
//		{
//			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadCubeMesh();
//			meshRenderer->SetMesh(sphereMesh);
//		}
//		{
//			shared_ptr<Shader> shader = make_shared<Shader>();
//			//shared_ptr<Texture> texture = make_shared<Texture>();
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


//#pragma region TestObject
//	shared_ptr<GameObject> obj0 = make_shared<GameObject>();
//	shared_ptr<GameObject> obj1 = make_shared<GameObject>();
//
//	
//
//	obj0->AddComponent(make_shared<Transform>());
//	obj1->AddComponent(make_shared<Transform>());
//
//	shared_ptr<Transform> transform0 = obj0->GetTransform();
//	
//	transform0->SetLocalPosition(Vec3(-100.f, 100.f, 200.f));
//	transform0->SetLocalScale(Vec3(100.f, 100.f, 1.f));
//
//	shared_ptr<Transform> transform1 = obj1->GetTransform();
//
//	transform1->SetLocalPosition(Vec3(100.f, 100.f, 200.f));
//	transform1->SetLocalScale(Vec3(100.f, 100.f, 1.f));
//	
//
//	shared_ptr<MeshRenderer> meshRenderer0 = make_shared<MeshRenderer>();
//	shared_ptr<MeshRenderer> meshRenderer1 = make_shared<MeshRenderer>();
//	{
//		shared_ptr<Mesh> mesh = make_shared<Mesh>();
//		mesh->Init(vertexVec, indexVec);
//		meshRenderer0->SetMesh(mesh);
//		meshRenderer1->SetMesh(mesh);
//	}
//
//	{
//		shared_ptr<Shader> shader = make_shared<Shader>();
//		shared_ptr<Material> material = make_shared<Material>();
//
//		shader->Init(L"..\\Resources\\Shader\\default.hlsl");
//		texture->Init(L"..\\Resources\\Texture\\veigar.jpg", "veigar", GEngine->GetSingleDescriptorAllocator());
//
//		material->SetShader(shader);
//		material->SetFloat(0, 0.3f);
//		material->SetFloat(1, 0.4f);
//		material->SetFloat(2, 0.3f);
//		material->SetTexture(0, texture->GetTextureHandle("veigar"));
//		meshRenderer0->SetMaterial(material);
//		meshRenderer1->SetMaterial(material);
//	}
//
//	obj0->AddComponent(meshRenderer0);
//	obj1->AddComponent(meshRenderer1);
//	
//
//	scene->AddGameObject(obj0);
//	scene->AddGameObject(obj1);
//#pragma endregion



	return scene;
}
