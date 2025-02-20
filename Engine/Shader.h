#pragma once
#include "Object.h"

enum class SHADER_TYPE : uint8
{
	DEFERRED,
	FORWARD,
	LIGHTING,
};

enum class RASTERIZER_TYPE : uint8
{
	CULL_NONE,
	CULL_FRONT,
	CULL_BACK,
	WIREFRAME,
};

enum class DEPTH_STENCIL_TYPE : uint8
{
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
	NO_DEPTH_TEST,			// ±íÀÌ Å×½ºÆ® X, ±íÀÌ ±â·Ï O
	NO_DEPTH_TEST_NO_WRITE, // ±íÀÌ Å×½ºÆ® X, ±íÀÌ ±â·Ï X
	LESS_NO_WRITE,			// ±íÀÌ Å×½ºÆ® O, ±íÀÌ ±â·Ï X
};

enum class BLEND_TYPE : uint8
{
	DEFAULT,			// µ¤¾î ¾´´Ù.
	ALPHA_BLEND,		// ¾ËÆÄ °ª°ú ºñ·ÊÇÏ¿© ¼¯´Â´Ù.
	ONE_TO_ONE_BLEND,	// 1 ´ë 1·Î ¼¯´Â´Ù.
	END,
};

struct ShaderInfo
{
	SHADER_TYPE						shaderType			= SHADER_TYPE::FORWARD;
	RASTERIZER_TYPE					rasterizerType		= RASTERIZER_TYPE::CULL_BACK;
	DEPTH_STENCIL_TYPE				depthStencilType	= DEPTH_STENCIL_TYPE::LESS;
	BLEND_TYPE						blendType			= BLEND_TYPE::DEFAULT;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE	topologyType		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
};

class Shader : public Object
{
public:
	Shader();
	virtual ~Shader();

	void Init(const wstring& path, ShaderInfo info = ShaderInfo(), const string& vs = "VS_Main", const string& ps = "PS_Main");
	void Update();

	SHADER_TYPE GetShaderType() const { return _info.shaderType; }

private:
	void CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode);
	void CreateVertexShader(const wstring& path, const string& name, const string& version);
	void CreatePixelShader(const wstring& path, const string& name, const string& version);


private:
	ShaderInfo			_info;

	ComPtr<ID3DBlob>	_vsBlob;
	ComPtr<ID3DBlob>	_psBlob;
	ComPtr<ID3DBlob>	_errBlob;

	ComPtr<ID3D12PipelineState>			_pipelineState;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC	_pipelineDesc = {};
};

