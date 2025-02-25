#pragma once
#include "Component.h"
#include "Object.h"

class Transform;
class MeshRenderer;
class MonoBehaviour;
class Camera;
class Light;
class ParticleSystem;

class GameObject : public Object, public enable_shared_from_this<GameObject>
{
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
	vector<shared_ptr<MonoBehaviour>> _scripts;

	uint8 _layerIndex = 0;

public:
	GameObject();
	virtual ~GameObject();

	void Init();

	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();

	shared_ptr<Component> GetFixedComponent(COMPONENT_TYPE type);

	shared_ptr<Transform>		GetTransform();
	shared_ptr<MeshRenderer>	GetMeshRenderer();
	shared_ptr<Camera>			GetCamera();
	shared_ptr<Light>			GetLight();
	shared_ptr<ParticleSystem>	GetParticleSystem();

	void AddComponent(shared_ptr<Component> component);

	void SetLayerIndex(uint8 index) { _layerIndex = index; }
	uint8 GetLayerIndex() const { return _layerIndex; }
};

