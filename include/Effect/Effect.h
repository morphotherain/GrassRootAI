#pragma once
#include <vector>
#include <memory>

#include "VSEffectComponent.h"
#include "PSEffectComponent.h"
#include "GSEffectComponent.h"

#include "VertexBufferComponent.h"
#include "ConstantBufferComponent.h"
#include "IndexBufferComponent.h"

#include "TexturesComponent.h"

#include "BlendStateComponent.h"
#include "RasterizerStateComponent.h"
#include "SamplerStateComponent.h"
#include "DepthStencilStateComponent.h"

class Effect {
public:
	Effect();
	~Effect() {}

	// 批量处理传递过来的GameObject数据，并进行渲染
	void Init();
	void apply();

	void initComponentIfNotNull(std::shared_ptr<EffectComponent> component);
	void applyEffectComponent(std::shared_ptr<EffectComponent> component);

	template<typename T>
	void addVertexShaderBuffer(std::wstring vsFileName, std::wstring csoFileName);

	void addPixelShader(std::wstring psFileName, std::wstring csoFileName);
	void addGeometryShader(std::wstring gsFileName, std::wstring csoFileName);

	template<typename T>
	void addConstantBuffer();

	void addTextures(std::vector<std::string> textureFileName);

	void addBlendState();
	void addSamplerState();

	void clearShader();

	template<typename T>
	std::shared_ptr<VSEffectComponent<T>> getVertexShader() { return std::dynamic_pointer_cast<VSEffectComponent<T>>(VS); }

	template<typename T>
	std::shared_ptr<VertexBufferComponent<T>> getVertexBuffer() { return std::dynamic_pointer_cast<VertexBufferComponent<T>>(VertexBuffer); }

	template<typename T>
	std::shared_ptr<ConstantBufferComponent<T>> getConstantBuffer() { return std::dynamic_pointer_cast<ConstantBufferComponent<T>>(ConstantBuffer); }

	std::shared_ptr<TexturesComponent> getTextures() { return std::dynamic_pointer_cast<TexturesComponent>(Textures); }

	std::shared_ptr<EffectComponent> VS;
	std::shared_ptr<EffectComponent> PS;
	std::shared_ptr<EffectComponent> GS;

	std::shared_ptr<EffectComponent> VertexBuffer;
	std::shared_ptr<EffectComponent> ConstantBuffer;
	std::shared_ptr<EffectComponent> IndexBuffer;

	std::shared_ptr<EffectComponent> Textures;

	std::shared_ptr<EffectComponent> BlendState;
	std::shared_ptr<EffectComponent> DepthStencilState;
	std::shared_ptr<EffectComponent> SamplerState;
	std::shared_ptr<EffectComponent> RasterizerState;
};

template<typename T>
inline void Effect::addConstantBuffer()
{
	ConstantBuffer = std::make_shared<ConstantBufferComponent<T>>();
}

template<typename T>
inline void Effect::addVertexShaderBuffer(std::wstring vsFileName, std::wstring csoFileName)
{
	VS = std::make_shared<VSEffectComponent<T>>(vsFileName, csoFileName);
	VertexBuffer = std::make_shared<VertexBufferComponent<T>>();
}