#include "pch.h"
#include "Game.h"
#include "Engine.h"

#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Transform.h"

shared_ptr<Texture> texture = make_shared<Texture>();

shared_ptr<GameObject> obj0 = make_shared<GameObject>();
shared_ptr<GameObject> obj1 = make_shared<GameObject>();

static XMFLOAT4 b0 = { 0.25f, 0.f, 0.1f, 0.f };
static XMFLOAT4 b1 = { 0.f, 0.f, 0.f, 0.f };
//static XMFLOAT4 b2 = { 0.f, 0.f, 0.1f, 0.f };
//static XMFLOAT4 b3 = { 0.f, 0.f, 0.f, 0.f };


void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

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

	vector<WORD> indexVec = {0, 1, 2, 0, 2, 3};

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

	GEngine->GetCmdQueue()->WaitSync();
}

void Game::Update()
{
	GEngine->Update();

	GEngine->RenderBegin();

	/*shader->Update();
	{
		

		

		
			

		
		mesh->Render(&b0, &elapsedTime, texture->GetTextureHandle("veigar")->srv);

		mesh->Render(&b2, &elapsedTime, texture->GetTextureHandle("veigar")->srv);
	}*/


	//XMFLOAT4 b0 = { 0.2f, 0.f, 0.1f, 1.0f };
	//XMFLOAT4 b1 = { 0.5f, 0.2f, 0.5f, 1.f };
	


	//XMFLOAT4 b2 = { -0.2f, 0.f, 0.0f, 1.f };
	//XMFLOAT4 b3 = { 1.0f, -0.2f, 1.f, 1.f };

	if (INPUT->GetButton(KEY_TYPE::W))
		b0.y += TIMER->GetDeltaTime();
	if (INPUT->GetButton(KEY_TYPE::S))
		b0.y -= TIMER->GetDeltaTime();
	if (INPUT->GetButton(KEY_TYPE::A))
		b1.x -= TIMER->GetDeltaTime();
	if (INPUT->GetButton(KEY_TYPE::D))
		b1.x += TIMER->GetDeltaTime();
	if (INPUT->GetButton(KEY_TYPE::UP))
		b1.z += TIMER->GetDeltaTime();
	if (INPUT->GetButton(KEY_TYPE::DOWN))
		b1.z -= TIMER->GetDeltaTime();

	obj0->GetTransform()->SetTransform(b0);
	obj1->GetTransform()->SetTransform(b1);
	
	obj0->Update();
	obj1->Update();

	GEngine->RenderEnd();

	GEngine->GetDescriptorPool()->Reset();
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->Reset();
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->Reset();
}