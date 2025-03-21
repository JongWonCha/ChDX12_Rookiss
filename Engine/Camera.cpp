#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "ParticleSystem.h"
#include "Engine.h"
#include "InstancingManager.h"

Matrix Camera::S_MatView;
Matrix Camera::S_MatViewInv;
Matrix Camera::S_MatProjection;

Camera::Camera() : Component(COMPONENT_TYPE::CAMERA)
{
}

Camera::~Camera()
{
}

void Camera::FinalUpdate()
{
	//auto a = GetTransform()->GetLocalToWorldMatrix();
	_matViewInv = GetTransform()->GetLocalToWorldMatrix();
	_matView = _matViewInv.Invert();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	if (_type == PROJECTION_TYPE::PERSPECTIVE)
		_matProjection = ::XMMatrixPerspectiveFovLH(_fov, width / height, _near, _far);
	else
		_matProjection = ::XMMatrixOrthographicLH(width * _scale, height * _scale, _near, _far);
}

//void Camera::Render()
//{
//	S_MatView = _matView;
//	S_MatProjection = _matProjection;
//
//	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
//
//	// TODO : Layer
//	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();
//
//	for (auto& gameObject : gameObjects)
//	{
//		if (gameObject->GetMeshRenderer() == nullptr)
//			continue;
//
//		if (IsCulled(gameObject->GetLayerIndex()))
//			continue;
//
//		gameObject->GetMeshRenderer()->Render();
//	}
//}

void Camera::SortGameObject()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	_vecForward.clear();
	_vecDeferred.clear();
	_vecParticle.clear();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr) continue;

		if(IsCulled(gameObject->GetLayerIndex())) continue;

		if (gameObject->GetMeshRenderer())
		{
			SHADER_TYPE shaderType = gameObject->GetMeshRenderer()->GetMaterial()->GetShader()->GetShaderType();
			switch (shaderType)
			{
			case SHADER_TYPE::FORWARD:
				_vecForward.emplace_back(gameObject);
				break;
			case SHADER_TYPE::DEFERRED:
				_vecDeferred.emplace_back(gameObject);
				break;
			}
		}
		else
		{
			if(gameObject->GetParticleSystem())
				_vecParticle.emplace_back(gameObject);
		}
	}
}

void Camera::Render_Forward()
{
	S_MatView = _matView;
	S_MatViewInv = _matViewInv;
	S_MatProjection = _matProjection;

	GET_SINGLE(InstancingManager)->Render(_vecForward);

	/*for (auto& gameObject : _vecForward)
	{
		gameObject->GetMeshRenderer()->Render();
	}*/

	for (auto& gameObject : _vecParticle)
	{
		gameObject->GetParticleSystem()->Render();
	}
}

void Camera::Render_Deferred()
{
	S_MatView = _matView;
	S_MatViewInv = _matViewInv;
	S_MatProjection = _matProjection;

	GET_SINGLE(InstancingManager)->Render(_vecDeferred);

	/*for (auto& gameObject : _vecDeferred)
	{
		gameObject->GetMeshRenderer()->Render();
	}*/
}
