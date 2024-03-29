#include "Triangle.hlsli"

// 顶点着色器


PSInput VS(VSInput input)
{
    PSInput output;
    output.position = float4(input.position, 1.0f);
    output.texCoord = input.texCoord;
    output.texIndex = input.texIndex; // 直接传递纹理索引
    return output;
}