#include "vertexData.h"

using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC PosTexIndex::inputLayout[3] =
{
// 位置字段
{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
// 纹理坐标字段
{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
// 纹理索引字段
{ "TEXINDEX", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

std::vector<PosTexIndex>  GenerateVertices(float x, float y, float deltaX, float deltaY) {

	std::vector<PosTexIndex> vertices;

	vertices.reserve(6); // 每个格子两个三角形，每个三角形3个顶点


	// 第一个三角形
	vertices.push_back({ XMFLOAT3(x, y, 0.0f),             XMFLOAT2(0.0f, 1.0f), 0 });
	vertices.push_back({ XMFLOAT3(x, (y + deltaY), 0.0f),       XMFLOAT2(0.0f, 0.0f), 0 });
	vertices.push_back({ XMFLOAT3((x + deltaX), y, 0.0f),       XMFLOAT2(1.0f, 1.0f), 0 });

	// 第二个三角形
	vertices.push_back({ XMFLOAT3((x + deltaX), y, 0.0f),       XMFLOAT2(1.0f, 1.0f), 0 });
	vertices.push_back({ XMFLOAT3(x, (y + deltaY), 0.0f),       XMFLOAT2(0.0f, 0.0f), 0 });
	vertices.push_back({ XMFLOAT3((x + deltaX), (y + deltaY), 0.0f), XMFLOAT2(1.0f, 0.0f), 0 });


	return vertices;
}

// 点的顶点布局描述
const D3D11_INPUT_ELEMENT_DESC PointVertexPosColor::inputLayout[3] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },            // pos
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },           // color
	{ "LUMINOSITY", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },               // luminosity
};

// 线条的顶点布局描述
const D3D11_INPUT_ELEMENT_DESC LineVertexPosColor::inputLayout[2] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },            // pos
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },           // color
};

const D3D11_INPUT_ELEMENT_DESC VertexPos::inputLayout[1] = {
	// 位置字段
	 { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC Pos2Tex::inputLayout[2] = {
	// 位置字段
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 纹理索引字段
	{ "TEXINDEX", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};