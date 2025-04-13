#include "StarMapPoint.hlsli"

// ������ɫ����������
[maxvertexcount(6)] // ����������������Ϊ6�����γ�����������
void GS_Point(point PointGSInput input[1], inout TriangleStream<PointGSInput> OutputStream)
{
    // ���ݹ�ȵ�����Ĵ�С�����Ը�����Ҫ����0.01f��0.1f��ϵ��
    //float pointSize = 0.03f + input[0].luminosity * 0.1f;
    float pointSize = 0.07f + log(1.0f + input[0].luminosity) * 0.15f;

    float aspectRatio = 0.5625f;

    // ������Ļ�ռ���ĸ�����ƫ�����������γ�һ������
    float2 offsets[4] = {
        float2(-pointSize * aspectRatio-2.0f, -pointSize - 2.0f), // ����
        float2(pointSize * aspectRatio + 2.0f, -pointSize - 2.0f),  // ����
        float2(pointSize * aspectRatio + 2.0f, pointSize + 2.0f),   // ����
        float2(-pointSize * aspectRatio - 2.0f, pointSize + 2.0f)   // ����
    };

    // �������������εĶ���˳���γ�һ������
    int indices[6] = { 0, 1, 2, 0, 2, 3 };

    // ͨ��������ɫ����չ��Ϊ���������ι��ɵľ���
    for (int i = 0; i < 6; ++i)
    {
        PointGSInput vertex = input[0];
        int index = indices[i];
        vertex.position.xy += offsets[index]; // Ӧ��ƫ����
        vertex.screenPos = offsets[index] / pointSize; // ����Բ�����ֵĹ�һ������
        OutputStream.Append(vertex);
    }
    OutputStream.RestartStrip(); // ���¿�ʼ�µ�ͼԪ
}
