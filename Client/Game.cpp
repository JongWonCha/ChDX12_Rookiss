#include "pch.h"
#include "Game.h"
#include "Engine.h"

shared_ptr<Mesh> mesh = make_shared<Mesh>();
shared_ptr<Shader> shader = make_shared<Shader>();
shared_ptr<Texture> texture = make_shared<Texture>();

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
	GEngine->RenderBegin();

	shader->Update();


	XMFLOAT4 b0 = { 0.25f, 0.f, 0.0f, 1.0f };
	XMFLOAT4 b1 = { 0.5f, 0.25f, 0.5f, 1.f };
	mesh->Render(&b0, &b1, texture->GetTextureHandle("veigar")->srv);


	XMFLOAT4 b2 = { -0.25f, 0.f, 0.f, 1.f };
	XMFLOAT4 b3 = { 1.0f, -0.25f, 1.f, 1.f };
	mesh->Render(&b2, &b3, texture->GetTextureHandle("veigar")->srv);

	GEngine->RenderEnd();

	GEngine->GetDescriptorPool()->Reset();
	GEngine->GetCB()->Reset();
}