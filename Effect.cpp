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

void Effect::addGeometryShader(std::wstring gsFileName, std::wstring csoFileName)
{
    GS = std::make_shared<GSEffectComponent>(gsFileName, csoFileName);
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

void Effect::clearShader()
{
    GS->clear();
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
    applyEffectComponent(ConstantBuffer);
    // 先设置渲染状态，确保渲染管线的配置完整
    applyEffectComponent(BlendState);
    applyEffectComponent(DepthStencilState);
    applyEffectComponent(RasterizerState);

    // 然后绑定纹理和采样器，以便着色器能够访问它们
    applyEffectComponent(Textures);
    applyEffectComponent(SamplerState);

    // 接着绑定各个着色器
    applyEffectComponent(VS);
    applyEffectComponent(PS);
    applyEffectComponent(GS);

    // 最后绑定缓冲区资源
    applyEffectComponent(IndexBuffer);
    applyEffectComponent(VertexBuffer);
}
