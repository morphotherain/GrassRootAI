#include "StarMapPoint.hlsli"

float4 PS_Point(PointGSInput input) : SV_TARGET
{
    // 计算当前像素点到圆心的距离
    float dist = length(input.screenPos);

// 使用 smoothstep 产生柔和的渐变边缘，扩大范围来增加光晕的大小
float alpha = 1.0f - smoothstep(0.0f, 2.0f, dist); // 控制光晕的总体范围

// 分段控制透明度，中心区域快速衰减，外围缓慢衰减
if (dist < 0.04f)
{
    // 中心区域非常亮，快速衰减
    alpha = pow(alpha, 20.0f); // 使用更高的幂函数，使中心亮度极高且快速衰减

    // 设置一个最小的透明度和亮度下限，确保星星在远处依然可见
    alpha = max(alpha, 0.05f);       // 防止透明度过低
}
else
{
    // 外围光晕区域，较慢的衰减
    alpha = 0.1f * pow(alpha, 8.0f); // 外围光晕衰减更柔和
}

// 控制亮度的渐变，中心更亮，外围光晕更淡
float brightness = lerp(1.0f, 1.5f, alpha); // 中心区域亮度增强，外围光晕维持较低亮度
brightness = max(brightness, 1.2f); // 保持最小亮度


// 最终颜色：调节亮度并应用透明度
float4 finalColor = input.color;// *brightness;
finalColor.a = alpha; // 设置透明度

// 丢弃几乎不可见的像素
if (finalColor.a < 0.001f)
    discard;

return finalColor;
}
