#pragma once
#include "Component.h"

enum class PROJECTION_TYPE
{
	PERSPECTIVE,
	ORTHOGRAPHIC
};

class Camera : public Component
{
public:
	Camera();
	virtual ~Camera();

	virtual void FinalUpdate() override;
	//void Render();



	void SetProjectionType(PROJECTION_TYPE type) { _type = type; }
	PROJECTION_TYPE GetProjectionType() { return _type; }

	void SortGameObject();
	void Render_Forward();
	void Render_Deferred();

	void SetCullingMaskLayerOnOff(uint8 layer, bool on)
	{
		if (on)
			_cullingMask |= (1 << layer);
		else
			_cullingMask &= ~(1 << layer);
	}

	void SetCullingMaskAll() { SetCullingMask(UINT32_MAX); }
	void SetCullingMask(uint32 mask) { _cullingMask = mask; }
	bool IsCulled(uint8 layer) { return (_cullingMask & (1 << layer)) != 0; }

private:
	PROJECTION_TYPE _type = PROJECTION_TYPE::PERSPECTIVE;

	float _near = 1.f;
	float _far = 1000.f;
	float _fov = XM_PI / 4.f; // 45 degree
	float _scale = 1.f;

	Matrix _matView = {};
	Matrix _matViewInv = {};
	Matrix _matProjection = {};

	uint32 _cullingMask = 0;// 비트와이즈 연산으로 레이어를 체크할 것이기 때문에 uint32로 선언, 비트 값이 0이면 렌더링 함.

private:
	vector<shared_ptr<GameObject>> _vecForward;
	vector<shared_ptr<GameObject>> _vecDeferred;
	vector<shared_ptr<GameObject>> _vecParticle;

public:
	// temp
	static Matrix S_MatView;
	static Matrix S_MatViewInv;
	static Matrix S_MatProjection;
};

