// ��Ķ�������ṹ��ƥ���Ĳ��֣�
struct PointVSInput
{
    float3 position : POSITION;      // ����ƥ�� POSITION �Ķ���
    float4 color : COLOR;            // ����ƥ�� COLOR �Ķ���
    float luminosity : LUMINOSITY;   // ����ƥ�� LUMINOSITY �Ķ���
};

// ������ɫ������
struct PointGSInput
{
    float4 position : SV_POSITION;   // �ü��ռ�λ�ã���Ӱ�첼��
    float2 screenPos : TEXCOORD;     // ����Բ������
    float luminosity : LUMINOSITY;   // ���
    float4 color : COLOR;            // ��ɫ
};
