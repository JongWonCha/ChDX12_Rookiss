#pragma once
#include "Component.h"

struct Constant_TransformMatrix
{
	XMFLOAT4 offset;
};

class Transform : public Component
{
public:
	Transform();
	virtual ~Transform();

	void Update() override;

	void SetTransform(const XMFLOAT4& transform);

	// TODO : �°� Parent/Chile ����

private:
	// TODO : World ��ġ ����
	Constant_TransformMatrix _transform;
};

