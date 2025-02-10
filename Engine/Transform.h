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

	// TODO : 온갖 Parent/Chile 관계

private:
	// TODO : World 위치 관련
	Constant_TransformMatrix _transform;
};

