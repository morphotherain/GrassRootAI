#include "StarMapPoint.hlsli"

float4 PS_Point(PointGSInput input) : SV_TARGET
{
    // ���㵱ǰ���ص㵽Բ�ĵľ���
    float dist = length(input.screenPos);

// ʹ�� smoothstep ������͵Ľ����Ե������Χ�����ӹ��εĴ�С
float alpha = 1.0f - smoothstep(0.0f, 2.0f, dist); // ���ƹ��ε����巶Χ

// �ֶο���͸���ȣ������������˥������Χ����˥��
if (dist < 0.04f)
{
    // ��������ǳ���������˥��
    alpha = pow(alpha, 20.0f); // ʹ�ø��ߵ��ݺ�����ʹ�������ȼ����ҿ���˥��

    // ����һ����С��͸���Ⱥ��������ޣ�ȷ��������Զ����Ȼ�ɼ�
    alpha = max(alpha, 0.05f);       // ��ֹ͸���ȹ���
}
else
{
    // ��Χ�������򣬽�����˥��
    alpha = 0.1f * pow(alpha, 8.0f); // ��Χ����˥�������
}

// �������ȵĽ��䣬���ĸ�������Χ���θ���
float brightness = lerp(1.0f, 1.5f, alpha); // ��������������ǿ����Χ����ά�ֽϵ�����
brightness = max(brightness, 1.2f); // ������С����


// ������ɫ���������Ȳ�Ӧ��͸����
float4 finalColor = input.color;// *brightness;
finalColor.a = alpha; // ����͸����

// �����������ɼ�������
if (finalColor.a < 0.001f)
    discard;

return finalColor;
}
