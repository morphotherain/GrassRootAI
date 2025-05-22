#pragma once
#include "EffectComponent.h"

class IndexBufferComponent : public EffectComponent
{
public:
	IndexBufferComponent();
	~IndexBufferComponent();

	virtual void Init();

	virtual void apply();
};