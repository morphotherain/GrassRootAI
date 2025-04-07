#include "ProcessCircle.hlsli"

// 像素着色器
Texture2DArray TexArray : register(t0);
SamplerState samLinear : register(s0);

float4 PS(PSInput input) : SV_Target
{
// 计算相对于纹理中心点的坐标
    float2 centerOffset = input.texCoord - float2(0.5f, 0.5f);


    float angle = atan2(-centerOffset.x, centerOffset.y);
    //angle += 3.1415926f;

    float res = (angle / 3.1415926f + 1.0f) / 2.0f;
    float maxAngle = input.texIndex;



    // 如果像素角度超过血量比例对应的角度
    if (res > maxAngle)
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f); // 丢弃该像素
    }

    // 进行纹理采样
    return TexArray.Sample(samLinear, float3(input.texCoord, 0.0f));
}