#include "Billboard.hlsli"

// 设置几何着色器的最大输出顶点数量为6，以形成两个三角形
[maxvertexcount(6)]
void main(point GeometryOutput input[1], inout TriangleStream<GeometryOutput> outputStream)
{
    // 定义公告板大小，可以根据需要调整这个大小
    float size = 0.02f;
    float factor = 16.0f / 9.0f; // 屏幕宽高比

    // 定义屏幕空间的四个顶点偏移量，用于形成一个矩形
    float2 halfSize = float2(size, size * factor);
    float2 offsets[4] = {
        float2(-halfSize.x, -halfSize.y),  // 左下
        float2(halfSize.x, -halfSize.y),   // 右下
        float2(halfSize.x, halfSize.y),    // 右上
        float2(-halfSize.x, halfSize.y)    // 左上
    };

    // 定义两组三角形的顶点顺序，以形成一个矩形
    int indices[6] = { 0, 1, 2, 0, 2, 3 };

    // 通过几何着色器扩展点为两个三角形构成的矩形
    for (int i = 0; i < 6; ++i)
    {
        GeometryOutput vertex = input[0];
        int index = indices[i];

        // 应用偏移量形成矩形的顶点
        vertex.position.xy += offsets[index]; // 调整顶点位置

        // 矫正后的纹理坐标，考虑宽高比因素
        float2 texCoord = offsets[index] / size + 0.5f;
        texCoord.y *= 1.0f / factor; // 调整纹理坐标的Y轴，矫正宽高比
        texCoord.y = 1.0f - texCoord.y;
        vertex.texCoord = texCoord;  // 更新纹理坐标

        // 传递纹理索引，保持纹理正确
        vertex.textureIndex = input[0].textureIndex;

        // 将顶点输出到图元流
        outputStream.Append(vertex);
    }
    // 不需要 RestartStrip，因为输出的顶点已经正确形成了两个三角形
    outputStream.RestartStrip(); // 重新开始新的图元
}
