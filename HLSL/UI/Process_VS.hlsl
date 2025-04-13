#include "ProcessCircle.hlsli"

// 顶点着色器
cbuffer MatrixBuffer : register(b0)
{
    //正解
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float texDelta;
};

PSInput VS(VSInput input)
{
    PSInput output;
    float4 worldPosition = mul(float4(input.position, 1.0f), modelMatrix); // 应用模型矩阵
    output.position = mul(worldPosition, viewMatrix); // 先将顶点位置乘以视图矩阵
    output.position = mul(output.position, projectionMatrix); // 再乘以投影矩阵
    output.texCoord = input.texCoord;
    output.texIndex = input.texIndex + texDelta;
    return output;
}