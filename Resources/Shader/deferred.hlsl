#ifndef _DEFAULT_HLSL_
#define _DEFAULT_HLSL_

#include "params.hlsl"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    
    row_major matrix matWorld : W;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    uint instanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;
    float3 worldPos : WORLDPOSITION;
    float3 worldNormal : WORLDNORMAL;
    float3 worldTangent : WORLDTANGENT;
    float3 worldBinormal : WORLDBINORMAL;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    
    // if Instancing Is On
    if(g_int_0 == 1)
    {
        output.pos = mul(float4(input.pos, 1.f), input.matWVP);
        output.uv = input.uv;

        output.viewPos = mul(float4(input.pos, 1.f), input.matWV).xyz;
        output.viewNormal = normalize(mul(float4(input.normal, 0.f), input.matWV).xyz);
        output.viewTangent = normalize(mul(float4(input.tangent, 0.f), input.matWV).xyz);
        output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
        

    }
    else
    {
        output.pos = mul(float4(input.pos, 1.f), g_matWVP);
        output.uv = input.uv;

        output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
        output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
        output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);
        output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
        
        output.worldNormal = normalize(mul(float4(input.normal, 0.f), g_matWorld).xyz);
        output.worldTangent = normalize(mul(float4(input.tangent, 0.f), g_matWorld).xyz);
        output.worldBinormal = normalize(cross(output.worldTangent, output.worldNormal));
       
    }
    output.worldPos = mul(float4(input.pos, 1.f), g_matWorld).xyz;
    
    return output;
}

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};

PS_OUT PS_Main(VS_OUT input)
{
    PS_OUT output = (PS_OUT) 0;

    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0) // Base Color Texture가 존재하는가?
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    float3 worldNormal = input.worldNormal;
    if (g_tex_on_1) // Normal Texture가 존재하는가?
    {
        // [0,255] 범위에서 [0,1]로 변환
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        // [0,1] 범위에서 [-1,1]로 변환
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
        
        float3x3 matWorldTBN = { input.worldTangent, input.worldBinormal, input.worldNormal };
        worldNormal = normalize(mul(tangentSpaceNormal, matWorldTBN));
    }
    
    output.position = float4(input.viewPos.xyz, 0.f);
    output.normal = float4(viewNormal, 0.f);
    
    // g_matViewInv[3].xyz : Camera Position
    float3 toEyeW = normalize(input.worldPos - g_matViewInv[3].xyz);
    float3 reflectVec = reflect(toEyeW, worldNormal.xyz);
    output.color = gCubeMap.Sample(g_sam_1, reflectVec);

    return output;
}

#endif