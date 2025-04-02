#include "Triangle.hlsli"

// 像素着色器
Texture2DArray TexArray : register(t0);
SamplerState samLinear : register(s0);

float4 PS(PSInput input) : SV_Target
{
    uint texIndex = (uint)input.texIndex; // 将浮点数转换为整数
    return TexArray.Sample(samLinear, float3(input.texCoord, texIndex));  
}

