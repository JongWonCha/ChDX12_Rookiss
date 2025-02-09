#pragma once

class Material;
class Mesh
{
public:
	void Init(const vector<Vertex>& vertexVec, const vector<WORD>& indexVec);
	void CreateVertexBuffer(const vector<Vertex>& vec);
	void CreateIndexBuffer(const vector<WORD>& vec);
	void Render(D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);


	UINT64 Fence();
	void WaitForFenceValue();

	~Mesh();
private:
	ID3D12CommandQueue*			_cmdQueue;
	ID3D12CommandAllocator*		_cmdAllocator;
	ID3D12GraphicsCommandList*	_cmdList;

	HANDLE			_hFenceEvent = nullptr;
	ID3D12Fence*	_pFence = nullptr;
	UINT64			_ui64FenceValue = 0;

	ComPtr<ID3D12Resource>		_vertexBuffer;			//GPU �޸�
	D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {};
	uint32						_vertexCount = 0;

	ComPtr<ID3D12Resource>		_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW		_indexBufferView = {};

};

