#include "Triangle.hlsli"

cbuffer TimeBuffer : register(b0)
{
    float frameTime;
};

// 像素着色器
Texture2DArray TexArray : register(t0);
SamplerState samLinear : register(s0);

float4 PS(PSInput input) : SV_Target
{
    // 使用当前时间来计算缩放因子，范围在[0, 2*pi]
    float scaleFactor = fmod(frameTime,240.0f) / 240.0f;
    float sinFactor = sin(frameTime / 240.0f) / 4.0f * sin(frameTime / 120.0f);
    float cosFactor = cos(frameTime / 240.0f) / 4.0f * sin(frameTime / 120.0f);

    float2 centerL = float2(0.5f + sinFactor, 0.5f + cosFactor); // 圆心位置，这里假设在纹理坐标(0.5, 0.5)处
    float2 centerR = float2(0.5f - sinFactor, 0.5f - cosFactor); // 圆心位置，这里假设在纹理坐标(0.5, 0.5)处
    float initialRadius = 0.25f; // 圆的初始半径
    float deltaRadius = 0.075f;

    // 根据缩放因子调整圆的半径
    float radius = initialRadius * scaleFactor;

    // 计算当前像素到圆心的距离
    float distanceToCenterL = length(input.texCoord - centerL);
    float distanceToCenterR = length(input.texCoord - centerR);

    float realDistanceL = fmod(distanceToCenterL + 2 * deltaRadius * scaleFactor, 2 * deltaRadius );
    float realDistanceR = fmod(distanceToCenterR + 2 * deltaRadius * scaleFactor, 2 * deltaRadius);

    // 如果像素到圆心的距离小于半径，就将像素设置为白色，否则设置为透明
    if (realDistanceL <= deltaRadius)
    {
        if (realDistanceR >= deltaRadius)
        {
            return float4(distanceToCenterL *3.0, distanceToCenterR * 3.0, (distanceToCenterL + distanceToCenterR) * 2.0f , 1.0f); // 白色
        }
        else
        {
            return float4(0.0f, 0.0f, 0.0f, 0.0f); // 透明
        }
    }
    else
    {
        if (realDistanceR <= deltaRadius)
        {
            return float4(distanceToCenterL * 3.0, distanceToCenterR * 3.0, (distanceToCenterL + distanceToCenterR) * 2.0f * sin(frameTime / 240.0f)+0.5f, 1.0f); // 白色
        }
        else
        {
            return float4(0.0f, 0.0f, 0.0f, 0.0f); // 透明
        }
    }
}

