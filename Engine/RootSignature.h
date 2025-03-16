#pragma once
class RootSignature
{
public:
	void Init();

	ComPtr<ID3D12RootSignature> GetGraphicsRootSignature() { return _graphicsRootSignature; }
	ComPtr<ID3D12RootSignature> GetComputeRootSignature() { return _computeRootSignature; }

private:
	void CreateSamplerDesc();
	void CreateGraphicsRootSignature();
	void CreateComputeRootSignature();

private:
	ComPtr<ID3D12RootSignature> _graphicsRootSignature;
	array<D3D12_STATIC_SAMPLER_DESC, 2> _samplerDescArray;
	D3D12_STATIC_SAMPLER_DESC	_samplerDesc;

	ComPtr<ID3D12RootSignature> _computeRootSignature;
};

