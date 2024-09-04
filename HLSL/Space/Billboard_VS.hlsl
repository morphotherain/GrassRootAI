#include "Billboard.hlsli"

GeometryOutput main(VertexInput input)
{
    GeometryOutput output;
    // 直接传递屏幕坐标
    output.position = float4(input.position, 0.0f, 1.0f); // Z设为0，W设为1
    output.texCoord = float2(0.0f, 0.0f); // 纹理坐标将在几何着色器中设置
    output.textureIndex = input.textureIndex;  // 传递纹理索引
    return output;
}
