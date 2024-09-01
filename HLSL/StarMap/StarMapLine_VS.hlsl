#include "StarMapLine.hlsli"

// �����������������任����
cbuffer MatrixBuffer : register(b0)
{
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// ������ɫ��
LinePSInput VS_Line(LineVSInput input)
{
    LinePSInput output;

    // Ӧ�ñ任����
    float4 worldPosition = mul(float4(input.position, 1.0f), modelMatrix);
    output.position = mul(worldPosition, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // ֱ�Ӵ�����ɫ
    output.color = input.color;

    return output;
}