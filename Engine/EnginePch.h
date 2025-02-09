#pragma once

#define _HAS_STD_BYTE 0

// include
#include <Windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/DirectXTex.inl"

// lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

// typedef
using int8  = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8  = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using Vec2 = XMFLOAT2;
using Vec3 = XMFLOAT3;
using Vec4 = XMFLOAT4;
using Matrix = XMMATRIX;

enum
{
	SWAP_CHAIN_BUFFER_COUNT = 2
};

struct WindowInfo
{
	HWND hwnd;
	int32 width;
	int32 height;
	bool windowed;
};

struct Vertex
{
	Vec3 pos;
	Vec4 color;
	Vec2 uv;
};

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

struct TEXTURE_HANDLE
{
	ComPtr<ID3D12Resource> pTexResource = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE srv = {};
};

class Engine;

#define DEVICE				GEngine->GetDevice()->GetDevice()
#define CMD_LIST			GEngine->GetCmdQueue()->GetCmdList()
#define ROOT_SIGNATURE		GEngine->GetRootSignature()->GetSignature()
#define RESOURCE_CMD_LIST	GEngine->GetCmdQueue()->GetResourceCmdList()

#define INPUT				GEngine->GetInput()
#define TIMER				GEngine->GetTimer()
#define DELTATIME			GEngine->GetTimer()->GetDeltaTime()

#define CONSTANTBUFFER(type) GEngine->GetCBByType(type)

extern unique_ptr<Engine> GEngine;