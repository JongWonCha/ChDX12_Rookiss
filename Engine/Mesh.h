#pragma once
#include "Object.h"

class Material;
class Mesh : public Object
{
public:
	void Init(const vector<Vertex>& vertexVec, const vector<uint32>& indexVec);
	void CreateVertexBuffer(const vector<Vertex>& vec);
	void CreateIndexBuffer(const vector<uint32>& vec);
	void Render(D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle, uint32 instanceCount = 1);


	UINT64 Fence();
	void WaitForFenceValue();

	Mesh();
	virtual ~Mesh();
private:
	ID3D12CommandQueue*			_cmdQueue;
	ID3D12CommandAllocator*		_cmdAllocator;
	ID3D12GraphicsCommandList*	_cmdList;

	HANDLE			_hFenceEvent = nullptr;
	ID3D12Fence*	_pFence = nullptr;
	UINT64			_ui64FenceValue = 0;

	ComPtr<ID3D12Resource>		_vertexBuffer;			//GPU ¸Þ¸ð¸®
	D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {};
	uint32						_vertexCount = 0;

	ComPtr<ID3D12Resource>		_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW		_indexBufferView = {};
	uint32						_indexCount = 0;
};

