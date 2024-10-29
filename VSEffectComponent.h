#pragma once
#include "EffectComponent.h"

template <typename VertexDataType>
class VSEffectComponent : public EffectComponent {
public:
    VSEffectComponent(std::wstring vsFileName, std::wstring csoFileName):m_vsFileName(vsFileName), m_csoFileName(csoFileName),PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST){}
    ~VSEffectComponent() {};

    virtual void Init();

    virtual void apply();

    virtual void clear();

    void SetVSFileName(std::wstring vsFileName) { m_vsFileName = vsFileName; };
    void SetCSOFileName(std::wstring csoFileName) { m_csoFileName = csoFileName; };
    void setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) { PrimitiveTopology = topology; };

    std::wstring m_vsFileName;
    std::wstring m_csoFileName;

    ComPtr<ID3D11VertexShader> m_pVertexShader;	
    ComPtr<ID3D11InputLayout> m_pVertexLayout;	// 顶点输入布局

    D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology;
};

template <typename VertexDataType>
void VSEffectComponent<VertexDataType>::Init()
{
    auto m_pd3dDevice = D3DManager::getInstance().getDevice();
    ComPtr<ID3DBlob> blob;

    HR(CreateShaderFromFile(m_csoFileName.c_str(), m_vsFileName.c_str(), "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
    HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));

    // 使用点的顶点着色器的 blob 创建点的顶点布局
    HR(m_pd3dDevice->CreateInputLayout(VertexDataType::inputLayout, ARRAYSIZE(VertexDataType::inputLayout),
        blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));
}

template <typename VertexDataType>
void VSEffectComponent<VertexDataType>::apply()
{
    auto m_pd3dImmediateContext = D3DManager::getInstance().getDeviceContext();
    m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get()); // 顶点布局
    m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0); // 线的顶点着色器
    m_pd3dImmediateContext->IASetPrimitiveTopology(PrimitiveTopology); // 图元拓扑
}

template<typename VertexDataType>
inline void VSEffectComponent<VertexDataType>::clear()
{
    auto m_pd3dImmediateContext = D3DManager::getInstance().getDeviceContext();
    m_pd3dImmediateContext->VSSetShader(nullptr, nullptr, 0); // 线的顶点着色器
}
