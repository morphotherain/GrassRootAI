#include "Billboard.hlsli"

GeometryOutput main(VertexInput input)
{
    GeometryOutput output;
    // ֱ�Ӵ�����Ļ����
    output.position = float4(input.position, 0.0f, 1.0f); // Z��Ϊ0��W��Ϊ1
    output.texCoord = float2(0.0f, 0.0f); // �������꽫�ڼ�����ɫ��������
    output.textureIndex = input.textureIndex;  // ������������
    return output;
}
