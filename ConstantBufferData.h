#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>


struct ConstantMVPIndex
{
	DirectX::XMMATRIX model;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
	float TexIndex; // 使用float来传递索引
	float padding[3]; // 添加padding以确保结构体的大小为16的倍数
};