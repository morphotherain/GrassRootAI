#pragma once
#include "EffectComponent.h"

class TexturesComponent : public EffectComponent
{
public:
	TexturesComponent(std::vector<std::string> textureFileNames) : textureFileNames(textureFileNames) {}
	~TexturesComponent() {};

	virtual void Init();

	virtual void apply();

	void setTextureFileNames(std::vector<std::string> textureFileNames) { this->textureFileNames = textureFileNames; }
	void addTextureFileName(std::string textureFileName) { this->textureFileNames.push_back(textureFileName); }

	ComPtr<ID3D11ShaderResourceView> textureArraySRV;
	ddsLoader m_ddsLoader;                                //材质加载
	std::vector<std::string> textureFileNames;
};