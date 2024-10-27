#include "GSEffectComponent.h"

void GSEffectComponent::Init()
{
	auto m_pd3dDevice = D3DManager::getInstance().getDevice();
	ComPtr<ID3DBlob> blob;

	HR(CreateShaderFromFile(m_csoFileName.c_str(), m_gsFileName.c_str(), "GS", "gs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pGeometryShader.GetAddressOf()));

}

void GSEffectComponent::apply()
{
	auto m_pd3dImmediateContext = D3DManager::getInstance().getDeviceContext();
	m_pd3dImmediateContext->GSSetShader(m_pGeometryShader.Get(), nullptr, 0); // 线的顶点着色器
}
