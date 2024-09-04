#include "Billboard.hlsli"

// 使用纹理数组
Texture2DArray textureArray : register(t0);
SamplerState samplerState : register(s0);

float4 main(GeometryOutput input) : SV_TARGET
{
    // 使用输入的纹理坐标和索引从纹理数组中采样
    float4 color = textureArray.Sample(samplerState, float3(input.texCoord, input.textureIndex));
    return color;
}
