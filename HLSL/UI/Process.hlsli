struct VSInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float texIndex : TEXINDEX;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float texIndex : TEXINDEX;
};