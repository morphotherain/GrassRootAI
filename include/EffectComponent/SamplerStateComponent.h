#pragma once
#include "EffectComponent.h"

class SamplerStateComponent : public EffectComponent
{
public:
	SamplerStateComponent() {};
	~SamplerStateComponent() {};

	virtual void Init();

	virtual void apply();

	ComPtr<ID3D11SamplerState> pSamplerState;
};
