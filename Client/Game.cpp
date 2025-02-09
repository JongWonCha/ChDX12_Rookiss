#include "pch.h"
#include "Game.h"
#include "Engine.h"

shared_ptr<Mesh> mesh = make_shared<Mesh>();
shared_ptr<Shader> shader = make_shared<Shader>();
shared_ptr<Texture> texture = make_shared<Texture>();

static XMFLOAT4 b0 = { 0.25f, 0.f, 0.1f, 0.f };
static XMFLOAT4 b1 = { 0.f, 0.f, 0.f, 0.f };
static XMFLOAT4 b2 = { 0.f, 0.f, 0.1f, 0.f };
static XMFLOAT4 b3 = { 0.f, 0.f, 0.f, 0.f };

static float elapsedTime = 0.0f;

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


	mesh->Init(vertexVec, indexVec);

	shader->Init(L"..\\Resources\\Shader\\default.hlsl");

	texture->Init(L"..\\Resources\\Texture\\veigar.jpg", "veigar", GEngine->GetSingleDescriptorAllocator());

	GEngine->GetCmdQueue()->WaitSync();
}

void Game::Update()
{
	GEngine->Update();

	GEngine->RenderBegin();

	shader->Update();
	{
		

		

		if (INPUT->GetButton(KEY_TYPE::W))
			b0.y += TIMER->GetDeltaTime();
		if (INPUT->GetButton(KEY_TYPE::S))
			b0.y -= TIMER->GetDeltaTime();
		if (INPUT->GetButton(KEY_TYPE::A))
			b2.x -= TIMER->GetDeltaTime();
		if (INPUT->GetButton(KEY_TYPE::D))
			b2.x += TIMER->GetDeltaTime();
		if (INPUT->GetButton(KEY_TYPE::UP))
			b2.z += TIMER->GetDeltaTime();
		if (INPUT->GetButton(KEY_TYPE::DOWN))
			b2.z -= TIMER->GetDeltaTime();
			

		elapsedTime += TIMER->GetDeltaTime();
		mesh->Render(&b0, &elapsedTime, texture->GetTextureHandle("veigar")->srv);

		mesh->Render(&b2, &elapsedTime, texture->GetTextureHandle("veigar")->srv);
	}


	//XMFLOAT4 b0 = { 0.2f, 0.f, 0.1f, 1.0f };
	//XMFLOAT4 b1 = { 0.5f, 0.2f, 0.5f, 1.f };
	


	//XMFLOAT4 b2 = { -0.2f, 0.f, 0.0f, 1.f };
	//XMFLOAT4 b3 = { 1.0f, -0.2f, 1.f, 1.f };
	

	GEngine->RenderEnd();

	GEngine->GetDescriptorPool()->Reset();
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->Reset();
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->Reset();
}