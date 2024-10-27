#pragma once
#include "TexturesComponent.h"

void TexturesComponent::Init()
{
	m_ddsLoader.Init(D3DManager::getInstance().getDevice(), D3DManager::getInstance().getDeviceContext());
	m_ddsLoader.InitTex32ArrayFromFiles(textureFileNames, textureArraySRV);
}

void TexturesComponent::apply()
{
	D3DManager::getInstance().getDeviceContext()->PSSetShaderResources(0, 1, textureArraySRV.GetAddressOf()); //绑定纹理
}
