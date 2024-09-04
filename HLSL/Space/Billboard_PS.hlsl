#include "Billboard.hlsli"

// ʹ����������
Texture2DArray textureArray : register(t0);
SamplerState samplerState : register(s0);

float4 main(GeometryOutput input) : SV_TARGET
{
    // ʹ�������������������������������в���
    float4 color = textureArray.Sample(samplerState, float3(input.texCoord, input.textureIndex));
    return color;
}
