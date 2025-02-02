#pragma once
class Mesh
{
public:
	void Init(vector<Vertex>& vec);
	void CreateVertexBuffer(const vector<Vertex>& vec);
	void Render(const XMFLOAT4 * b0, const XMFLOAT4*  b1);

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

	ComPtr<ID3D12Resource>		_vertexBuffer;			//GPU ¸Þ¸ð¸®
	D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {};
	uint32						_vertexCount = 0;
};

