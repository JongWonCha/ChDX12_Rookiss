#include "pch.h"
#include "Game.h"
#include "Engine.h"

#include "SceneManager.h"



static XMFLOAT4 b0 = { 0.25f, 0.f, 0.1f, 0.f };
static XMFLOAT4 b1 = { 0.f, 0.f, 0.f, 0.f };
//static XMFLOAT4 b2 = { 0.f, 0.f, 0.1f, 0.f };
//static XMFLOAT4 b3 = { 0.f, 0.f, 0.f, 0.f };


void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

	GET_SINGLE(SceneManager)->LoadScene(L"TestScene");
}

void Game::Update()
{
	GEngine->Update();

	//GEngine->RenderBegin();

	/*shader->Update();
	{
		

		

		
			

		
		mesh->Render(&b0, &elapsedTime, texture->GetTextureHandle("veigar")->srv);

		mesh->Render(&b2, &elapsedTime, texture->GetTextureHandle("veigar")->srv);
	}*/


	//XMFLOAT4 b0 = { 0.2f, 0.f, 0.1f, 1.0f };
	//XMFLOAT4 b1 = { 0.5f, 0.2f, 0.5f, 1.f };
	


	//XMFLOAT4 b2 = { -0.2f, 0.f, 0.0f, 1.f };
	//XMFLOAT4 b3 = { 1.0f, -0.2f, 1.f, 1.f };

	/*if (INPUT->GetButton(KEY_TYPE::W))
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
	obj1->Update();*/

	//GEngine->RenderEnd();

	GEngine->GetDescriptorPool()->Reset();
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->Reset();
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->Reset();
}