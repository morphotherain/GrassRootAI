#include "SamplerStateComponent.h"

void SamplerStateComponent::Init()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;  // 使用点采样
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;   // 禁用U方向上的循环
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;   // 禁用V方向上的循环
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;   // 禁用W方向上的循环，对3D纹理有效
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HR(D3DManager::getInstance().getDevice()->CreateSamplerState(&sampDesc, pSamplerState.GetAddressOf()));
}

void SamplerStateComponent::apply()
{
	// 绑定采样器状态到像素着色器
	D3DManager::getInstance().getDeviceContext()->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());
}