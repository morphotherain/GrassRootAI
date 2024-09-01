struct LineVSInput
{
    float3 position : POSITION;      // 必须匹配 POSITION 的定义
    float4 color : COLOR;            // 必须匹配 COLOR 的定义
};

// 线条的像素输入
struct LinePSInput
{
    float4 position : SV_POSITION;   // 裁剪空间位置
    float4 color : COLOR;            // 颜色
}; 
