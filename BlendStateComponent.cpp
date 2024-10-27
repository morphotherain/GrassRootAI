#pragma once
#include "BlendStateComponent.h"

void BlendStateComponent::Init()
{
	D3D11_BLEND_DESC blendDesc = { 0 };
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// 创建混合状态对象
	HR(D3DManager::getInstance().getDevice()->CreateBlendState(&blendDesc, m_pBlendState.GetAddressOf()));
}

void BlendStateComponent::apply()
{
	float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	D3DManager::getInstance().getDeviceContext()->OMSetBlendState(m_pBlendState.Get(), blendFactor, 0xffffffff);
}
