#pragma once
#include "Component.h"

enum class LIGHT_TYPE : uint8
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};

struct LightColor
{
	Vec4 diffuse;
	Vec4 ambient;
	Vec4 specular;
};

struct LightInfo
{
	LightColor	color;		// 48
	Vec4		position;	// 16
	Vec4		direction;	// 16
	int32		lightType;	// 4
	float		range;		// 4
	float		angle;		// 4
	int32		padding;	// 4
	// 16 byte aligned
};

struct LightParams
{
	uint32		lightCount;	// 4
	Vec3		padding;	// 12
	LightInfo	lights[50];	// 
};

class Light : public Component
{
public:
	Light();
	virtual ~Light();

	virtual void FinalUpdate() override;

public:
	const LightInfo& GetLightInfo() { return _lightInfo; }

	void SetLightDirection(const Vec3& direction)	{ _lightInfo.direction = direction; }

	void SetDiffuse(const Vec3& diffuse)			{ _lightInfo.color.diffuse = diffuse; }
	void SetAmbient(const Vec3& ambient)			{ _lightInfo.color.diffuse = ambient; }
	void SetSpecular(const Vec3& specular)			{ _lightInfo.color.diffuse = specular; }

	void SetLightType(LIGHT_TYPE type)				{ _lightInfo.lightType = static_cast<uint8>(type); }
	void SetLightRange(float range)					{ _lightInfo.range = range; }
	void SetLightAngle(float angle)					{ _lightInfo.angle = angle; }
private:
	LightInfo _lightInfo = {};
};