#include "Effect.h"

void Effect::initComponentIfNotNull(std::shared_ptr<EffectComponent> component) {
    if (component.get() != nullptr) {
        component->Init();
    }
}

void Effect::applyEffectComponent(std::shared_ptr<EffectComponent> component)
{
    if (component.get() != nullptr) {
        component->apply();
    }
}


Effect::Effect()
{
}



void Effect::addPixelShader(std::wstring psFileName, std::wstring csoFileName)
{
    PS = std::make_shared<PSEffectComponent>(psFileName, csoFileName);
}

void Effect::addTextures(std::vector<std::string> textureFileName)
{
    Textures = std::make_shared<TexturesComponent>(textureFileName);
}

void Effect::addBlendState()
{
    BlendState = std::make_shared<BlendStateComponent>();
}

void Effect::addSamplerState()
{
    SamplerState = std::make_shared<SamplerStateComponent>();
}




void Effect::Init()
{
    initComponentIfNotNull(VS);
    initComponentIfNotNull(PS);
    initComponentIfNotNull(GS);
    initComponentIfNotNull(VertexBuffer);
    initComponentIfNotNull(ConstantBuffer);
    initComponentIfNotNull(IndexBuffer);
    initComponentIfNotNull(Textures);
    initComponentIfNotNull(BlendState);
    initComponentIfNotNull(DepthStencilState);
    initComponentIfNotNull(SamplerState);
    initComponentIfNotNull(RasterizerState);

}

void Effect::apply() {
    applyEffectComponent(VS);
    applyEffectComponent(PS);
    applyEffectComponent(GS);
    applyEffectComponent(VertexBuffer);
    applyEffectComponent(ConstantBuffer);
    applyEffectComponent(IndexBuffer);
    applyEffectComponent(Textures);
    applyEffectComponent(BlendState);
    applyEffectComponent(DepthStencilState);
    applyEffectComponent(SamplerState);
    applyEffectComponent(RasterizerState);


}