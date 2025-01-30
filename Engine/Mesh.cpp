#include "pch.h"
#include "Mesh.h"
#include "Engine.h"

void Mesh::Init(vector<Vertex>& vec)
{
	_vertexCount = static_cast<uint32>(vec.size());

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	DEVICE->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));
	DEVICE->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));
	DEVICE->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));

	_cmdList->Close();

	DEVICE->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_pFence));

	_ui64FenceValue = 0;

	_hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	CreateVertexBuffer(vec);
}

void Mesh::CreateVertexBuffer(const vector<Vertex>& vec)
{
	ID3D12Resource* pUploadBuffer = nullptr;
	UINT vertexBufferSize = sizeof(Vertex) * _vertexCount;

	DEVICE->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer)
	);

	if (!vec.empty())
	{
		_cmdAllocator->Reset();
		_cmdList->Reset(_cmdAllocator, nullptr);

		DEVICE->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&pUploadBuffer)
		);

		void* pVertexDataBegin = nullptr;
		CD3DX12_RANGE writeRange(0, 0);

		HRESULT hr;
		hr = pUploadBuffer->Map(0, &writeRange, reinterpret_cast<void**>(&pVertexDataBegin));
		if (FAILED(hr))
		{
			__debugbreak();
			return;
		}

		memcpy(pVertexDataBegin, &vec[0], vertexBufferSize);
		pUploadBuffer->Unmap(0, nullptr);

		_cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition((_vertexBuffer.Get()), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
		_cmdList->CopyBufferRegion(_vertexBuffer.Get(), 0, pUploadBuffer, 0, vertexBufferSize);
		_cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		_cmdList->Close();

		ID3D12CommandList* ppCmdLists[] = { _cmdList };
		_cmdQueue->ExecuteCommandLists(_countof(ppCmdLists), ppCmdLists);

		Fence();
		WaitForFenceValue();
	}

	_vertexBufferView.BufferLocation = _vertexBuffer.Get()->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex);
	_vertexBufferView.SizeInBytes = vertexBufferSize;

	if (pUploadBuffer)
	{
		pUploadBuffer->Release();
		pUploadBuffer = nullptr;
	}
}

void Mesh::Render()
{
	CMD_LIST->SetGraphicsRootSignature(ROOT_SIGNATURE.Get());
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView);
	CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);
}

UINT64 Mesh::Fence()
{
	_ui64FenceValue++;
	_cmdQueue->Signal(_pFence, _ui64FenceValue);
	return _ui64FenceValue;
}

void Mesh::WaitForFenceValue()
{
	const UINT64 expectedFenceValue = _ui64FenceValue;

	if (_pFence->GetCompletedValue() < expectedFenceValue)
	{
		_pFence->SetEventOnCompletion(expectedFenceValue, _hFenceEvent);
		WaitForSingleObject(_hFenceEvent, INFINITE);
	}
}

Mesh::~Mesh()
{
	if (_pFence)
	{
		WaitForFenceValue();
	}
	
	if (_cmdQueue)
	{
		_cmdQueue->Release();
		_cmdQueue = nullptr;
	}
	if (_cmdList)
	{
		_cmdList->Release();
		_cmdList = nullptr;
	}
	if (_cmdAllocator)
	{
		_cmdAllocator->Release();
		_cmdAllocator = nullptr;
	}
	if (_hFenceEvent)
	{
		CloseHandle(_hFenceEvent);
		_hFenceEvent = nullptr;
	}
	if (_pFence)
	{
		_pFence->Release();
		_pFence = nullptr;
	}
}
