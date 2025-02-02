#include "pch.h"
#include "Game.h"
#include "Engine.h"

shared_ptr<Mesh> mesh = make_shared<Mesh>();
shared_ptr<Shader> shader = make_shared<Shader>();

void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

	vector<Vertex> vec(3);
	vec[0].pos = Vec3(0.f, 0.25f, 0.25f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[1].pos = Vec3(0.25f, -0.25f, 0.25f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[2].pos = Vec3(-0.25f, -0.25f, 0.25f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);

	mesh->Init(vec);

	shader->Init(L"..\\Resources\\Shader\\default.hlsl");

	GEngine->GetCmdQueue()->WaitSync();
}

void Game::Update()
{
	GEngine->RenderBegin();

	shader->Update();

	XMFLOAT4 b0 = { 0.25f, 0.75f, 0.0f, 1.0f };
	XMFLOAT4 b1 = { 0.5f, 0.5f, 0.5f, 1.f };
	mesh->Render(&b0, &b1);


	XMFLOAT4 b2 = { -0.25f, -0.25f, 0.0f, 1.0f };
	XMFLOAT4 b3 = { 1.0f, 1.f, 1.f, 1.f };
	mesh->Render(&b2, &b3);

	GEngine->RenderEnd();

	GEngine->GetDescriptorPool()->Reset();
	GEngine->GetCB()->Reset();
}