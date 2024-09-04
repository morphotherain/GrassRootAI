#include "StarMapPoint.hlsli"

// 常量缓冲区，包含变换矩阵
cbuffer MatrixBuffer : register(b0)
{
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float3 cameraPosition; // 摄像机位置
    float padding;         // 用于填充，使得常量缓冲区对齐

};

// 顶点着色器
PointGSInput VS_Point(PointVSInput input)
{
    PointGSInput output;

    // 应用变换矩阵
    float4 worldPosition = mul(float4(input.position, 1.0f), modelMatrix);
    output.position = mul(worldPosition, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // 传递光度和颜色
    output.luminosity = input.luminosity * 1.0f;
    output.color = input.color;//* input.luminosity; // 光度应用于颜色

    return output;
}
