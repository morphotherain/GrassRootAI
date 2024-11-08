#include "Billboard.hlsli"

// ���ü�����ɫ������������������Ϊ6�����γ�����������
[maxvertexcount(6)]
void main(point GeometryOutput input[1], inout TriangleStream<GeometryOutput> outputStream)
{
    // ���幫����С�����Ը�����Ҫ���������С
    float size = 0.02f;
    float factor = 16.0f / 9.0f; // ��Ļ��߱�

    // ������Ļ�ռ���ĸ�����ƫ�����������γ�һ������
    float2 halfSize = float2(size, size * factor);
    float2 offsets[4] = {
        float2(-halfSize.x, -halfSize.y),  // ����
        float2(halfSize.x, -halfSize.y),   // ����
        float2(halfSize.x, halfSize.y),    // ����
        float2(-halfSize.x, halfSize.y)    // ����
    };

    // �������������εĶ���˳�����γ�һ������
    int indices[6] = { 0, 1, 2, 0, 2, 3 };

    // ͨ��������ɫ����չ��Ϊ���������ι��ɵľ���
    for (int i = 0; i < 6; ++i)
    {
        GeometryOutput vertex = input[0];
        int index = indices[i];

        // Ӧ��ƫ�����γɾ��εĶ���
        vertex.position.xy += offsets[index]; // ��������λ��

        // ��������������꣬���ǿ�߱�����
        float2 texCoord = offsets[index] / size + 0.5f;
        texCoord.y *= 1.0f / factor; // �������������Y�ᣬ������߱�
        texCoord.y = 1.0f - texCoord.y;
        vertex.texCoord = texCoord;  // ������������

        // ������������������������ȷ
        vertex.textureIndex = input[0].textureIndex;

        // �����������ͼԪ��
        outputStream.Append(vertex);
    }
    // ����Ҫ RestartStrip����Ϊ����Ķ����Ѿ���ȷ�γ�������������
    outputStream.RestartStrip(); // ���¿�ʼ�µ�ͼԪ
}
