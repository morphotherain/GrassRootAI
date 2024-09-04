cbuffer ConstantBuffer : register(b0)
{
    matrix viewProjMatrix;  // ��ͼͶӰ�������ڼ�����ɫ���е��������
};

struct VertexInput
{
    float2 position : POSITION; // 2D ��Ļ����
    float textureIndex : TEXINDEX;  // �������������
};

struct GeometryOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float textureIndex : TEXINDEX;
};
