#pragma once
#include "EffectComponent.h"

template <typename VertexDataType>
class VertexBufferComponent : public EffectComponent {
public:
    VertexBufferComponent();
    ~VertexBufferComponent();


    virtual void Init();
    virtual void apply();

    void setVertices(const std::vector<VertexDataType>& _vertices) { m_vertices = _vertices; }
	std::vector<VertexDataType> getVertices() { return m_vertices; }
    UINT getVerticsCount() { return (UINT)m_vertices.size(); }
private:

    ComPtr<ID3D11Buffer> m_pVertexBuffer;		// 顶点缓冲区
    std::vector<VertexDataType> m_vertices;			// 顶点数据
};



template <typename VertexDataType>
VertexBufferComponent<VertexDataType>::VertexBufferComponent() : EffectComponent() {}

template <typename VertexDataType>
VertexBufferComponent<VertexDataType>::~VertexBufferComponent() {}

template <typename VertexDataType>
void VertexBufferComponent<VertexDataType>::Init() {
	auto m_pd3dDevice = D3DManager::getInstance().getDevice();

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexDataType) * m_vertices.size(); // 注意这里的变化
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = m_vertices.data();
	HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

}

template <typename VertexDataType>
void VertexBufferComponent<VertexDataType>::apply() {
	auto m_pd3dImmediateContext = D3DManager::getInstance().getDeviceContext();
	UINT stride = sizeof(VertexDataType);
	UINT offset = 0;
	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	m_pd3dImmediateContext->Draw(m_vertices.size(), 0);
}
