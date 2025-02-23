#ifndef _COMPUTE_HLSL_
#define _COMPUTE_HLSL_

#include "params.hlsl"

RWTexture2D<float4> g_rwtex_0 : register(u0);

// 쓰레드 그룹 당 쓰레드 개수
// max : 1024 (CS_5.0 버전에서) : [numthreads(x, y, z)] 에서 x * y * z가 1024를 넘기면 안된다.
// 하나의 쓰레드 그룹은 하나의 다중 처리기에서 실행

// ex ) 그룹 갯수 (5, 3 ,2), 그룹 당 쓰레드 갯수 (10, 8, 3)
// 그룹 (2, 1, 0) 의 (7, 5, 0) 쓰레드의 예시
// SV_GroupThreadID : (7, 5, 0)
// SV_GroupID : (2, 1, 0)
// SV_DispatchThreadID : [(2, 1, 0) * (10, 8, 3)] + (7, 5, 0) = (27, 13, 0)
// SV_GroupIndex : (0 * 10 * 8) + (5 * 10) + 7 = 57
[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    if (threadIndex.y % 2 == 0)
        g_rwtex_0[threadIndex.xy] = float4(1.f, 0.f, 0.f, 1.f);
    else
        g_rwtex_0[threadIndex.xy] = float4(0.f, 1.f, 0.f, 1.f);
}

#endif