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

#pragma region TestObject
	shared_ptr<GameObject> obj0 = make_shared<GameObject>();
	shared_ptr<GameObject> obj1 = make_shared<GameObject>();

	vector<Vertex> vertexVec(4);
	vertexVec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vertexVec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vertexVec[0].uv = Vec2(0.f, 0.f);
	vertexVec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vertexVec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vertexVec[1].uv = Vec2(1.f, 0.f);
	vertexVec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vertexVec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vertexVec[2].uv = Vec2(1.f, 1.f);
	vertexVec[3].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vertexVec[3].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vertexVec[3].uv = Vec2(0.f, 1.f);

	vector<WORD> indexVec = { 0, 1, 2, 0, 2, 3 };

	obj0->AddComponent(make_shared<Transform>());
	obj1->AddComponent(make_shared<Transform>());

	shared_ptr<Transform> transform0 = obj0->GetTransform();
	
	transform0->SetLocalPosition(Vec3(-100.f, 100.f, 200.f));
	transform0->SetLocalScale(Vec3(100.f, 100.f, 1.f));

	shared_ptr<Transform> transform1 = obj1->GetTransform();

	transform1->SetLocalPosition(Vec3(100.f, 100.f, 200.f));
	transform1->SetLocalScale(Vec3(100.f, 100.f, 1.f));
	

	shared_ptr<MeshRenderer> meshRenderer0 = make_shared<MeshRenderer>();
	shared_ptr<MeshRenderer> meshRenderer1 = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->Init(vertexVec, indexVec);
		meshRenderer0->SetMesh(mesh);
		meshRenderer1->SetMesh(mesh);
	}

	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shared_ptr<Material> material = make_shared<Material>();

		shader->Init(L"..\\Resources\\Shader\\default.hlsl");
		texture->Init(L"..\\Resources\\Texture\\veigar.jpg", "veigar", GEngine->GetSingleDescriptorAllocator());

		material->SetShader(shader);
		material->SetFloat(0, 0.3f);
		material->SetFloat(1, 0.4f);
		material->SetFloat(2, 0.3f);
		material->SetTexture(0, texture->GetTextureHandle("veigar"));
		meshRenderer0->SetMaterial(material);
		meshRenderer1->SetMaterial(material);
	}

	obj0->AddComponent(meshRenderer0);
	obj1->AddComponent(meshRenderer1);
	

	scene->AddGameObject(obj0);
	scene->AddGameObject(obj1);
#pragma endregion

#pragma region Camera
	shared_ptr<GameObject> camera = make_shared<GameObject>();
	camera->AddComponent(make_shared<Transform>());
	camera->AddComponent(make_shared<Camera>());
	camera->AddComponent(make_shared<TestCameraScript>());
	camera->GetTransform()->SetLocalPosition(Vec3(0.f, 100.f, 0.f));
	scene->AddGameObject(camera);
#pragma endregion

	return scene;
}
