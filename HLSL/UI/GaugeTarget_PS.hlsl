#include "Gauge.hlsli"

// 像素着色器
Texture2DArray TexArray : register(t0);
SamplerState samLinear : register(s0);

float4 PS(PSInput input) : SV_Target
{
    float2 centerOffset = input.texCoord - float2(0.5f, 0.5f);


    float angle = atan2(centerOffset.x, -centerOffset.y);

    float res = (angle / 3.1415926f + 1.0f) / 2.0f;
    float scale = 0.1335f;
    float maxAngle = input.texIndex;

    // 检查res是否在(scale, 1 - scale)范围内
    if (res > scale && res < 1 - scale) {
        // 进行线性映射
        float mappedRes = (res - scale) / (1 - 2 * scale);
        // 如果像素角度超过血量比例对应的角度
        if (mappedRes > maxAngle)
        {
            return float4(0.0f, 0.0f, 0.0f, 0.0f); // 丢弃该像素
        }

        // 进行纹理采样
        return TexArray.Sample(samLinear, float3(input.texCoord, 0.0f));
    }
    else {
        return float4(0.0f, 0.0f, 0.0f, 0.0f); // 丢弃该像素
    }

}