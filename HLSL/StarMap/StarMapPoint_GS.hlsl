#include "StarMapPoint.hlsli"

// 几何着色器输入和输出
[maxvertexcount(6)] // 调整最大输出顶点数为6，以形成两个三角形
void GS_Point(point PointGSInput input[1], inout TriangleStream<PointGSInput> OutputStream)
{
    // 根据光度调整点的大小，可以根据需要调整0.01f和0.1f的系数
    //float pointSize = 0.03f + input[0].luminosity * 0.1f;
    float pointSize = 0.07f + log(1.0f + input[0].luminosity) * 0.15f;

    float aspectRatio = 0.5625f;

    // 定义屏幕空间的四个顶点偏移量，用于形成一个矩形
    float2 offsets[4] = {
        float2(-pointSize * aspectRatio-2.0f, -pointSize - 2.0f), // 左下
        float2(pointSize * aspectRatio + 2.0f, -pointSize - 2.0f),  // 右下
        float2(pointSize * aspectRatio + 2.0f, pointSize + 2.0f),   // 右上
        float2(-pointSize * aspectRatio - 2.0f, pointSize + 2.0f)   // 左上
    };

    // 定义两组三角形的顶点顺序，形成一个矩形
    int indices[6] = { 0, 1, 2, 0, 2, 3 };

    // 通过几何着色器扩展点为两个三角形构成的矩形
    for (int i = 0; i < 6; ++i)
    {
        PointGSInput vertex = input[0];
        int index = indices[i];
        vertex.position.xy += offsets[index]; // 应用偏移量
        vertex.screenPos = offsets[index] / pointSize; // 用于圆形遮罩的归一化坐标
        OutputStream.Append(vertex);
    }
    OutputStream.RestartStrip(); // 重新开始新的图元
}
