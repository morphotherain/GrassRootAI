// 点的顶点输入结构（匹配点的布局）
struct PointVSInput
{
    float3 position : POSITION;      // 必须匹配 POSITION 的定义
    float4 color : COLOR;            // 必须匹配 COLOR 的定义
    float luminosity : LUMINOSITY;   // 必须匹配 LUMINOSITY 的定义
};

// 几何着色器输入
struct PointGSInput
{
    float4 position : SV_POSITION;   // 裁剪空间位置，不影响布局
    float2 screenPos : TEXCOORD;     // 用于圆形遮罩
    float luminosity : LUMINOSITY;   // 光度
    float4 color : COLOR;            // 颜色
};
