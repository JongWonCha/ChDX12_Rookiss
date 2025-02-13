#include "pch.h"
#include "Mesh.h"
#include "Engine.h"
#include "Transform.h"
#include "Material.h"

Mesh::Mesh() : Object(OBJECT_TYPE::MESH)
{
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

void Mesh::Init(const vector<Vertex>& vertexVec, const vector<uint32>& indexVec)
{
	_vertexCount = static_cast<uint32>(vertexVec.size());
	_indexCount = static_cast<uint32>(indexVec.size());

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

	CreateVertexBuffer(vertexVec);
	CreateIndexBuffer(indexVec);

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

void Mesh::CreateIndexBuffer(const vector<uint32>& vec)
{
	HRESULT hr = S_OK;

	ComPtr<ID3D12Resource>	pUploadBuffer = nullptr;
	uint32					indexBufferSize = sizeof(uint32) * vec.size();

	hr = DEVICE->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&_indexBuffer)
	);

	if (FAILED(hr))
	{
		__debugbreak();
		return;
	}

	if (!vec.empty())
	{
		if (FAILED(_cmdAllocator->Reset()))
			__debugbreak();

		if (FAILED(_cmdList->Reset(_cmdAllocator, nullptr)))
			__debugbreak();

		hr = DEVICE->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&pUploadBuffer)
		);

		if (FAILED(hr))
		{
			__debugbreak();
			return;
		}

		void* pIndexDataBegin = nullptr;
		CD3DX12_RANGE writeRange(0, 0);

		hr = pUploadBuffer->Map(0, &writeRange, reinterpret_cast<void**>(&pIndexDataBegin));
		if (FAILED(hr))
		{
			__debugbreak();
			return;
		}
		memcpy(pIndexDataBegin, &vec[0], indexBufferSize);
		pUploadBuffer->Unmap(0, nullptr);

		_cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_indexBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
		_cmdList->CopyBufferRegion(_indexBuffer.Get(), 0, pUploadBuffer.Get(), 0, indexBufferSize);
		_cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_indexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

		_cmdList->Close();

		ID3D12CommandList* ppCmdLists[] = { _cmdList };
		_cmdQueue->ExecuteCommandLists(_countof(ppCmdLists), ppCmdLists);

		Fence();
		WaitForFenceValue();
	}

	_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	_indexBufferView.SizeInBytes = indexBufferSize;
	_indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	/*if (pUploadBuffer)
	{
		pUploadBuffer->Release();
		pUploadBuffer = nullptr;
	}*/
}

void Mesh::Render(D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
{
	//shared_ptr<DescriptorPool> dp = GEngine->GetDescriptorPool();
	//shared_ptr<ConstantBuffer> cbTransform = GEngine->GetCBByType(CONSTANT_BUFFER_TYPE::TRANSFORM);
	//shared_ptr<ConstantBuffer> cbMaterial = GEngine->GetCBByType(CONSTANT_BUFFER_TYPE::MATERIAL);


	//CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescTable = {};
	//CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescTable = {};

	//dp->AllocDescriptorTable(&cpuDescTable, &gpuDescTable, 3);

	//CB_CONTAINER* cbContainer0 = cbTransform->Alloc();
	//if (!cbContainer0)
	//{
	//	__debugbreak();
	//	return;
	//}

	//Constant_TransformMatrix* cb0 = (Constant_TransformMatrix*)cbContainer0->pSystemMemAddr;
	//cb0->offset = b0->offset;

	//CB_CONTAINER* cbContainer1 = cbMaterial->Alloc();
	//if (!cbContainer1)
	//{
	//	__debugbreak();
	//	return;
	//}

	//Constant_MaterialParams* cb1 = (Constant_MaterialParams*)cbContainer1->pSystemMemAddr;
	//*cb1 = *time;
	///*cb1->SetAllInt(0);
	//cb1->SetAllFloat(*time);*/

	//UINT srvDescriptorSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//CMD_LIST->SetGraphicsRootSignature(ROOT_SIGNATURE.Get());

	//CMD_LIST->SetDescriptorHeaps(1, dp->GetDescriptorHeap().GetAddressOf());

	/*CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDesc0(cpuDescTable, 0, srvDescriptorSize);
	DEVICE->CopyDescriptorsSimple(1, cbvDesc0, cbContainer0->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDesc1(cpuDescTable, 1, srvDescriptorSize);
	DEVICE->CopyDescriptorsSimple(1, cbvDesc1, cbContainer1->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	if (srv.ptr)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvDest(cpuDescTable, 2, srvDescriptorSize);
		DEVICE->CopyDescriptorsSimple(1, srvDest, srv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}*/

	// 1. CopyDescriptors »ç¿ë
	/*UINT numDescriptors[] = { 1, 1, srv.ptr ? 1 : 0 };
	D3D12_CPU_DESCRIPTOR_HANDLE dstHandles[] = {
		cpuDescTable,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuDescTable, 1, srvDescriptorSize),
		CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuDescTable, 2, srvDescriptorSize)
	};
	D3D12_CPU_DESCRIPTOR_HANDLE srcHandles[] = {
		cbContainer0->CBVHandle,
		cbContainer1->CBVHandle,
		srv
	};

	UINT numRanges = srv.ptr ? 3 : 2;
	DEVICE->CopyDescriptors(
		numRanges, dstHandles, numDescriptors,
		numRanges, srcHandles, numDescriptors,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
	);*/

	CMD_LIST->SetGraphicsRootDescriptorTable(0, gpuHandle);

	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView);
	CMD_LIST->IASetIndexBuffer(&_indexBufferView);
	CMD_LIST->DrawIndexedInstanced(_indexCount, 1, 0, 0, 0);
	//CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);
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


