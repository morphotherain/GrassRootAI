cbuffer ConstantBuffer : register(b0)
{
    matrix viewProjMatrix;  // 视图投影矩阵，用于几何着色器中的深度修正
};

struct VertexInput
{
    float2 position : POSITION; // 2D 屏幕坐标
    float textureIndex : TEXINDEX;  // 顶点的纹理索引
};

struct GeometryOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float textureIndex : TEXINDEX;
};
