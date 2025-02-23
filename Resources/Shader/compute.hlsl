#ifndef _COMPUTE_HLSL_
#define _COMPUTE_HLSL_

#include "params.hlsl"

RWTexture2D<float4> g_rwtex_0 : register(u0);

// ������ �׷� �� ������ ����
// max : 1024 (CS_5.0 ��������) : [numthreads(x, y, z)] ���� x * y * z�� 1024�� �ѱ�� �ȵȴ�.
// �ϳ��� ������ �׷��� �ϳ��� ���� ó���⿡�� ����

// ex ) �׷� ���� (5, 3 ,2), �׷� �� ������ ���� (10, 8, 3)
// �׷� (2, 1, 0) �� (7, 5, 0) �������� ����
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