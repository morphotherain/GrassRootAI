#include "StarMapPoint.hlsli"

// �����������������任����
cbuffer MatrixBuffer : register(b0)
{
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float3 cameraPosition; // �����λ��
    float padding;         // ������䣬ʹ�ó�������������

};

// ������ɫ��
PointGSInput VS_Point(PointVSInput input)
{
    PointGSInput output;

    // Ӧ�ñ任����
    float4 worldPosition = mul(float4(input.position, 1.0f), modelMatrix);
    output.position = mul(worldPosition, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // ���ݹ�Ⱥ���ɫ
    output.luminosity = input.luminosity * 1.0f;
    output.color = input.color;//* input.luminosity; // ���Ӧ������ɫ

    return output;
}
