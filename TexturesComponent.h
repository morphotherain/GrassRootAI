#pragma once
#include "EffectComponent.h"

class TexturesComponent : public EffectComponent
{
public:
	TexturesComponent(std::vector<std::string> textureFileNames): textureFileNames(textureFileNames) {}
	~TexturesComponent() {};


	virtual void Init();

	virtual void apply();

	void settextureFileNames(std::vector<std::string> textureFileNames) { this->textureFileNames = textureFileNames; }

	ComPtr<ID3D11ShaderResourceView> textureArraySRV;
	ddsLoader m_ddsLoader;                                //材质加载
	std::vector<std::string> textureFileNames;
};