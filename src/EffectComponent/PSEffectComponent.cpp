#include "PSEffectComponent.h"

void PSEffectComponent::Init()
{
	auto m_pd3dDevice = D3DManager::getInstance().getDevice();
	ComPtr<ID3DBlob> blob;

	HR(CreateShaderFromFile(m_csoFileName.c_str(), m_psFileName.c_str(), "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));
}

void PSEffectComponent::apply()
{
	auto m_pd3dImmediateContext = D3DManager::getInstance().getDeviceContext();
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0); // 线的顶点着色器
}

void PSEffectComponent::clear()
{
	auto m_pd3dImmediateContext = D3DManager::getInstance().getDeviceContext();
	m_pd3dImmediateContext->PSSetShader(nullptr, nullptr, 0); // 线的顶点着色器
}