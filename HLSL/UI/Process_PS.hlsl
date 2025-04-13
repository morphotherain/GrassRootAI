#include "ProcessCircle.hlsli"

// 像素着色器
Texture2DArray TexArray : register(t0);
SamplerState samLinear : register(s0);

float4 PS(PSInput input) : SV_Target
{
    // 直接使用纹理坐标的X分量作为进度判断（水平方向）
    float progress = input.texIndex; // 进度值范围[0.0, 1.0]

    // 水平进度条：当当前像素的X坐标超过进度值时隐藏
    if (input.texCoord.x > progress)
    {
        // 超出进度部分完全透明
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    // 在进度范围内的像素正常采样纹理
    float4 color = TexArray.Sample(samLinear, float3(input.texCoord.xy, 0.0f));

    // 可选：添加颜色渐变效果
    // color.rgb *= lerp(float3(1.0,0,0), float3(0,1.0,0), progress);

return color;
}