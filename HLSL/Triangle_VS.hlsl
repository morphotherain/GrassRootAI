#include "Triangle.hlsli"

// 顶点着色器
cbuffer MatrixBuffer : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
};

PSInput VS(VSInput input)
{
    PSInput output;
    float4 worldPosition = float4(input.position, 1.0f);
    output.position = mul(worldPosition, viewMatrix); // 先将顶点位置乘以视图矩阵
    output.position = mul(output.position, projectionMatrix); // 再乘以投影矩阵
    output.texCoord = input.texCoord;
    output.texIndex = input.texIndex;
    return output;
}