struct LineVSInput
{
    float3 position : POSITION;      // ����ƥ�� POSITION �Ķ���
    float4 color : COLOR;            // ����ƥ�� COLOR �Ķ���
};

// ��������������
struct LinePSInput
{
    float4 position : SV_POSITION;   // �ü��ռ�λ��
    float4 color : COLOR;            // ��ɫ
}; 
