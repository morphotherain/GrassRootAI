#include "StarMapLine.hlsli"

// 常量缓冲区，包含变换矩阵
cbuffer MatrixBuffer : register(b0)
{
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// 顶点着色器
LinePSInput VS_Line(LineVSInput input)
{
    LinePSInput output;

    // 应用变换矩阵
    float4 worldPosition = mul(float4(input.position, 1.0f), modelMatrix);
    output.position = mul(worldPosition, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // 直接传递颜色
    output.color = input.color;

    return output;
}