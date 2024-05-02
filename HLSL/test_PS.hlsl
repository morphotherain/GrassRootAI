#include "Triangle.hlsli"

cbuffer TimeBuffer : register(b0)
{
    float frameTime;
};

// ������ɫ��
Texture2DArray TexArray : register(t0);
SamplerState samLinear : register(s0);

float4 PS(PSInput input) : SV_Target
{
    // ʹ�õ�ǰʱ���������������ӣ���Χ��[0, 2*pi]
    float scaleFactor = fmod(frameTime,240.0f) / 240.0f;
    float sinFactor = sin(frameTime / 240.0f) / 4.0f * sin(frameTime / 120.0f);
    float cosFactor = cos(frameTime / 240.0f) / 4.0f * sin(frameTime / 120.0f);

    float2 centerL = float2(0.5f + sinFactor, 0.5f + cosFactor); // Բ��λ�ã������������������(0.5, 0.5)��
    float2 centerR = float2(0.5f - sinFactor, 0.5f - cosFactor); // Բ��λ�ã������������������(0.5, 0.5)��
    float initialRadius = 0.25f; // Բ�ĳ�ʼ�뾶
    float deltaRadius = 0.075f;

    // �����������ӵ���Բ�İ뾶
    float radius = initialRadius * scaleFactor;

    // ���㵱ǰ���ص�Բ�ĵľ���
    float distanceToCenterL = length(input.texCoord - centerL);
    float distanceToCenterR = length(input.texCoord - centerR);

    float realDistanceL = fmod(distanceToCenterL + 2 * deltaRadius * scaleFactor, 2 * deltaRadius );
    float realDistanceR = fmod(distanceToCenterR + 2 * deltaRadius * scaleFactor, 2 * deltaRadius);

    // ������ص�Բ�ĵľ���С�ڰ뾶���ͽ���������Ϊ��ɫ����������Ϊ͸��
    if (realDistanceL <= deltaRadius)
    {
        if (realDistanceR >= deltaRadius)
        {
            return float4(distanceToCenterL *3.0, distanceToCenterR * 3.0, (distanceToCenterL + distanceToCenterR) * 2.0f , 1.0f); // ��ɫ
        }
        else
        {
            return float4(0.0f, 0.0f, 0.0f, 0.0f); // ͸��
        }
    }
    else
    {
        if (realDistanceR <= deltaRadius)
        {
            return float4(distanceToCenterL * 3.0, distanceToCenterR * 3.0, (distanceToCenterL + distanceToCenterR) * 2.0f * sin(frameTime / 240.0f)+0.5f, 1.0f); // ��ɫ
        }
        else
        {
            return float4(0.0f, 0.0f, 0.0f, 0.0f); // ͸��
        }
    }
}

