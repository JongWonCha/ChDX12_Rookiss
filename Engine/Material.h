#pragma once

#include "Object.h"

class Shader;
class Texture;

enum
{
	MATERIAL_INT_COUNT = 4,
	MATERIAL_FLOAT_COUNT = 4,
	MATERIAL_TEXTURE_COUNT = 4,
	MATERIAL_VECTOR2_COUNT = 4,
	MATERIAL_VECTOR4_COUNT = 4,
};



struct Constant_MaterialParams
{
	void SetInt(uint8 index, int32 value) { intParams[index] = value; }
	void SetFloat(uint8 index, float value) { floatParams[index] = value; }
	void SetTexOn(uint8 index, int32 value) { texOnParams[index] = value; }
	void SetVec2(uint8 index, Vec2 value) { vec2Params[index] = value; }
	void SetVec4(uint8 index, Vec4 value) { vec4Params[index] = value; }

	void SetAllInt(int32 value)
	{
		for (int i = 0; i < MATERIAL_INT_COUNT; ++i)
			intParams[i] = value;
	}

	void SetAllFloat(float value)
	{
		for (int i = 0; i < MATERIAL_FLOAT_COUNT; ++i)
			floatParams[i] = value;
	}

	void SetAllVec2(Vec2 value)
	{
		for (int i = 0; i < MATERIAL_VECTOR2_COUNT; ++i)
			vec2Params[i] = value;
	}

	void SetAllVec4(Vec4 value)
	{
		for (int i = 0; i < MATERIAL_VECTOR4_COUNT; ++i)
			vec4Params[i] = value;
	}

	array<int32, MATERIAL_INT_COUNT> intParams;
	array<float, MATERIAL_FLOAT_COUNT> floatParams;
	array<int32, MATERIAL_TEXTURE_COUNT> texOnParams;
	array<Vec2, MATERIAL_VECTOR2_COUNT> vec2Params;
	array<Vec4, MATERIAL_VECTOR4_COUNT> vec4Params;
};

class Material : public Object
{
	shared_ptr<Shader>		_shader;
	Constant_MaterialParams	_params;
	array<shared_ptr<Texture>, MATERIAL_TEXTURE_COUNT> _textures;

public:
	shared_ptr<Shader> GetShader() { return _shader; }

	void SetShader(shared_ptr<Shader> shader) { _shader = shader; }
	void SetInt(uint8 index, int32 value) { _params.SetInt(index, value); }
	void SetFloat(uint8 index, float value) { _params.SetFloat(index, value); }
	void SetTexture(uint8 index, shared_ptr<Texture> texture)
	{
		_textures[index] = texture;
		_params.SetTexOn(index, (texture == nullptr) ? 0 : 1);
	}

	void SetVec2(uint8 index, Vec2 value)
	{
		_params.SetVec2(index, value); 
	}

	void SetVec4(uint8 index, Vec4 value)
	{
		_params.SetVec4(index, value);
	}

	void PushGraphicsData();
	void PushComputeData();
	void Dispatch(uint32 x, uint32 y, uint32 z);

	Material();
	virtual ~Material();
};

