#include "pch.h"
#include "SceneManager.h"

#include "Scene.h"
#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"

shared_ptr<Texture> texture = make_shared<Texture>();

void SceneManager::Update()
{
	if (_activeScene == nullptr)
		return;

	_activeScene->Update();
	_activeScene->LateUpdate();
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

	shared_ptr<GameObject> obj0 = make_shared<GameObject>();
	shared_ptr<GameObject> obj1 = make_shared<GameObject>();



	vector<Vertex> vertexVec(4);
	vertexVec[0].pos = Vec3(-0.25f, 0.25f, 0.0f);
	vertexVec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vertexVec[0].uv = Vec2(0.f, 0.f);
	vertexVec[1].pos = Vec3(0.25f, 0.25f, 0.0f);
	vertexVec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vertexVec[1].uv = Vec2(1.f, 0.f);
	vertexVec[2].pos = Vec3(0.25f, -0.25f, 0.0f);
	vertexVec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vertexVec[2].uv = Vec2(1.f, 1.f);
	vertexVec[3].pos = Vec3(-0.25f, -0.25f, 0.0f);
	vertexVec[3].color = Vec4(1.f, 1.f, 0.f, 1.f);
	vertexVec[3].uv = Vec2(0.f, 1.f);

	vector<WORD> indexVec = { 0, 1, 2, 0, 2, 3 };

	obj0->Init();
	obj1->Init();

	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->Init(vertexVec, indexVec);
		meshRenderer->SetMesh(mesh);
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
		meshRenderer->SetMaterial(material);
	}

	obj0->AddComponent(meshRenderer);
	obj1->AddComponent(meshRenderer);

	scene->AddGameObject(obj0);
	scene->AddGameObject(obj1);

	return scene;
}
