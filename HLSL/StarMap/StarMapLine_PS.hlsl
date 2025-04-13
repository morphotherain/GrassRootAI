#include "StarMapLine.hlsli"

// ����������ɫ��
float4 PS_Line(LinePSInput input) : SV_TARGET
{
    // ����������ɫ��ʹ��䰵����������
    float4 lineColor = input.color * 0.1f; // �������ֵ���Կ������ȣ�0.7f ʹ����΢�䰵

    // ��ȡ��ǰ���ص�͸���ȿ��ƣ�ģ�⿹���Ч��
    float edgeSoftness = 0.5; // ��Ե�ữ�̶ȣ������Լ��پ�ݸ�
    float distToEdge = abs(input.position.x); // ���� position.x ��ʾ���������ĵľ���

    // ʹ�� smoothstep ʵ�ֿ���ݣ�����͸����
    float alpha = smoothstep(edgeSoftness - 0.1f, edgeSoftness, distToEdge); // ���ƿ��������

    // �ʵ��������Ȳ���ӷ���Ч��
    float glow = smoothstep(edgeSoftness, edgeSoftness + 0.2f, distToEdge) * 0.2f; // ����Ч��΢��

    // ������ɫ�ϳɣ������������ɫ�ں�
    float4 finalColor = lineColor * (1.0f - glow) + float4(1.0f, 1.0f, 1.0f, 1.0f) * glow;

    // Ӧ�ÿ���ݺ��͸����
    finalColor.a *= alpha;

    // ���������ɫ
    return finalColor;
}
