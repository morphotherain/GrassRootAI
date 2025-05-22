#pragma once
#include "EffectComponent.h"

class DepthStencilStateComponent : public EffectComponent
{
public:
	DepthStencilStateComponent();
	~DepthStencilStateComponent();

	virtual void Init();

	virtual void apply();

	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;        // 深度模板缓冲区
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;   // 渲染目标视图
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;   // 深度模板视图
};