#pragma once
#include "Component.h"

class Mesh;
class Material;
class InstancingBuffer;

union InstanceID
{
	struct
	{
		uint32 meshID;
		uint32 materialID;
	};
	uint64 id;
};

class MeshRenderer : public Component
{
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }

	shared_ptr<Material> GetMaterial() { return _material; }

	virtual void Update() override;

	void Render();
	void Render(shared_ptr<InstancingBuffer>& buffer);

	uint64 GetInstanceID();
private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;
};

