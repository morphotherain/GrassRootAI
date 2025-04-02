#include "Gauge.hlsli"

// 像素着色器
Texture2DArray TexArray : register(t0);
SamplerState samLinear : register(s0);

float4 PS(PSInput input) : SV_Target
{
// 计算相对于纹理中心点的坐标
    float2 centerOffset = input.texCoord - float2(0.5f, 0.5f);


    // 计算像素到中心点的角度
    float angle = atan2(centerOffset.y, centerOffset.x);
    angle += 3.1415926f;

    // 计算血量比例对应的最大角度，假设 input.texIndex 是 0 到 1 之间的比例值
    float maxAngle = 3.1415926f * input.texIndex;

    // 如果像素角度超过血量比例对应的角度
    if (angle > maxAngle)
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f); // 丢弃该像素
    }

    // 进行纹理采样
    return TexArray.Sample(samLinear, float3(input.texCoord, 0.0f));
}