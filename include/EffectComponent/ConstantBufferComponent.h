#pragma once
#include "EffectComponent.h"
#include "ConstantBufferData.h"

template <typename ConstantBufferType>
class ConstantBufferComponent : public EffectComponent
{
public:
	ConstantBufferComponent() {};
	~ConstantBufferComponent() {};

	virtual void Init();

	virtual void apply();

	ConstantBufferType* Map();

	void Unmap();

	// 创建常量缓冲区
	ComPtr<ID3D11Buffer> matrixBuffer;
};

template <typename ConstantBufferType>
void ConstantBufferComponent<ConstantBufferType>::Init()
{
	auto m_pd3dDevice = D3DManager::getInstance().getDevice();
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ConstantBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// 使用设备创建缓冲区
	auto hr = m_pd3dDevice->CreateBuffer(&matrixBufferDesc, nullptr, matrixBuffer.GetAddressOf());
}

template <typename ConstantBufferType>
void ConstantBufferComponent<ConstantBufferType>::apply()
{
	// 设置顶点着色器中的常量缓冲区
	D3DManager::getInstance().getDeviceContext()->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());
}

template<typename ConstantBufferType>
ConstantBufferType* ConstantBufferComponent<ConstantBufferType>::Map()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	auto m_pd3dImmediateContext = D3DManager::getInstance().getDeviceContext();
	HR(m_pd3dImmediateContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	return (ConstantBufferType*)mappedResource.pData;
}

template<typename ConstantBufferType>
void ConstantBufferComponent<ConstantBufferType>::Unmap()
{
	auto m_pd3dImmediateContext = D3DManager::getInstance().getDeviceContext();
	m_pd3dImmediateContext->Unmap(matrixBuffer.Get(), 0);
}
