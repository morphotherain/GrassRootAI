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



#endif