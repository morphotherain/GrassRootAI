#pragma once
#include "EffectComponent.h"

class RasterizerStateComponent : public EffectComponent
{
public:
    RasterizerStateComponent();
    ~RasterizerStateComponent();


    virtual void Init();

    virtual void apply();
};