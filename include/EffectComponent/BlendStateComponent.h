#pragma once
#include "EffectComponent.h"

class BlendStateComponent : public EffectComponent
{
public:
	BlendStateComponent() {};
	~BlendStateComponent() {};

	virtual void Init();

	virtual void apply();

	ComPtr<ID3D11BlendState> m_pBlendState;	// 混合状态对象
};