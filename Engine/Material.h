#pragma once

#include "Object.h"

class Shader;
class Texture;

enum
{
	MATERIAL_INT_COUNT = 5,
	MATERIAL_FLOAT_COUNT = 5,
	MATERIAL_TEXTURE_COUNT = 5,
};



struct Constant_MaterialParams
{
	void SetInt(uint8 index, int32 value) { intParams[index] = value; }
	void SetFloat(uint8 index, float value) { floatParams[index] = value; }

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

	array<int32, MATERIAL_INT_COUNT> intParams;
	array<float, MATERIAL_FLOAT_COUNT> floatParams;
};

class Material : public Object
{
	shared_ptr<Shader>		_shader;
	Constant_MaterialParams	_params;
	array<const TEXTURE_HANDLE*, MATERIAL_TEXTURE_COUNT> _textures;

public:
	shared_ptr<Shader> GetShader() { return _shader; }

	void SetShader(shared_ptr<Shader> shader) { _shader = shader; }
	void SetInt(uint8 index, int32 value) { _params.SetInt(index, value); }
	void SetFloat(uint8 index, float value) { _params.SetFloat(index, value); }
	void SetTexture(uint8 index, const TEXTURE_HANDLE* textureSrv) { _textures[index] = textureSrv; }

	void PushData();

	Material();
	virtual ~Material();
};

