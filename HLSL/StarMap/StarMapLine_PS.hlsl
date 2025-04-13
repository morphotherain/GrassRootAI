#include "StarMapLine.hlsli"

// 线条像素着色器
float4 PS_Line(LinePSInput input) : SV_TARGET
{
    // 调整线条颜色，使其变暗，减少亮度
    float4 lineColor = input.color * 0.1f; // 调整这个值可以控制亮度，0.7f 使其稍微变暗

    // 获取当前像素的透明度控制，模拟抗锯齿效果
    float edgeSoftness = 0.5; // 边缘柔化程度，调整以减少锯齿感
    float distToEdge = abs(input.position.x); // 假设 position.x 表示离线条中心的距离

    // 使用 smoothstep 实现抗锯齿，控制透明度
    float alpha = smoothstep(edgeSoftness - 0.1f, edgeSoftness, distToEdge); // 控制抗锯齿区域

    // 适当降低亮度并添加发光效果
    float glow = smoothstep(edgeSoftness, edgeSoftness + 0.2f, distToEdge) * 0.2f; // 发光效果微调

    // 最终颜色合成，发光和线条颜色融合
    float4 finalColor = lineColor * (1.0f - glow) + float4(1.0f, 1.0f, 1.0f, 1.0f) * glow;

    // 应用抗锯齿后的透明度
    finalColor.a *= alpha;

    // 输出最终颜色
    return finalColor;
}
