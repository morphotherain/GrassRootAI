#ifndef POS_TEX_INDEX_H
#define POS_TEX_INDEX_H
#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>

struct PosTexIndex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 TexCoor;
	float index;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};

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


#endif