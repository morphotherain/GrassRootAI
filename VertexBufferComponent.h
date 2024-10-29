#pragma once
#include "EffectComponent.h"

template <typename VertexDataType>
class VertexBufferComponent : public EffectComponent {
public:
    VertexBufferComponent();
    ~VertexBufferComponent();


    virtual void Init();
    virtual void apply();

	VertexDataType* Map();

	void Unmap();

    void setVertices(const std::vector<VertexDataType>& _vertices) { m_vertices = _vertices; }
	std::vector<VertexDataType> getVertices() { return m_vertices; }
    UINT getVerticsCount() { return (UINT)m_vertices.size(); }
	void setCPUAccessFlags(D3D11_CPU_ACCESS_FLAG _CPUAccessFlags) { CPUAccessFlags = _CPUAccessFlags; }
    void setUsage(D3D11_USAGE _usage) { usage = _usage; }
private:

    ComPtr<ID3D11Buffer> m_pVertexBuffer;		// 顶点缓冲区
    std::vector<VertexDataType> m_vertices;			// 顶点数据

	D3D11_CPU_ACCESS_FLAG CPUAccessFlags = static_cast<D3D11_CPU_ACCESS_FLAG>(0);
	D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE;
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
	vbd.Usage = usage;
	vbd.ByteWidth = sizeof(VertexDataType) * m_vertices.size(); // 注意这里的变化
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = CPUAccessFlags;
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


template<typename VertexDataType>
VertexDataType* VertexBufferComponent<VertexDataType>::Map()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	auto m_pd3dImmediateContext = D3DManager::getInstance().getDeviceContext();
	HR(m_pd3dImmediateContext->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	return (VertexDataType*)mappedResource.pData;
}

template<typename VertexDataType>
void VertexBufferComponent<VertexDataType>::Unmap()
{
	auto m_pd3dImmediateContext = D3DManager::getInstance().getDeviceContext();
	m_pd3dImmediateContext->Unmap(m_pVertexBuffer.Get(), 0);
}

