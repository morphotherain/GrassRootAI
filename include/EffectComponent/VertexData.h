﻿#ifndef POS_TEX_INDEX_H
#define POS_TEX_INDEX_H
#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>
#include <vector>

struct PosTexIndex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 TexCoor;
	float index;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};
std::vector<PosTexIndex>  GenerateVertices(float x, float y, float delta_x, float delta_y);

struct PointVertexPosColor
{
	DirectX::XMFLOAT3 pos;            // 顶点位置
	DirectX::XMFLOAT4 color;          // 颜色 (RGBA)，16字节对齐
	float luminosity;                 // 光度

	// 输入布局描述
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};

struct LineVertexPosColor
{
	DirectX::XMFLOAT3 pos;            // 顶点位置，12 字节
	DirectX::XMFLOAT4 color;          // 颜色 (RGBA)，16 字节

	// 输入布局描述
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};

// 顶点结构，包含一个 3D 位置
struct VertexPos
{
	DirectX::XMFLOAT3 PosL; // 立方体顶点的局部空间位置
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[1];
};

struct Pos2Tex {
	DirectX::XMFLOAT2 position;  // 2D 屏幕坐标
	float texIndex;              // 纹理索引
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};

#endif