#include "D3DManager.h"

void D3DManager::clearRenderTarget(float color[4]) {
	if (m_renderTargetView) {
		m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), color);
	}
}

void D3DManager::clearDepthStencil(float depth, UINT8 stencil) {
	if (m_depthStencilView) {
		m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
	}
}

void D3DManager::setRenderTarget(ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView) {
	m_renderTargetView = renderTargetView;
	m_depthStencilView = depthStencilView;
	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
}

void D3DManager::present(bool vsync) {
	m_swapChain->Present(vsync ? 1 : 0, 0);
}

void D3DManager::cleanup() {
	// Release all managed D3D resources
	m_renderTargetView.Reset();
	m_depthStencilView.Reset();
	m_swapChain.Reset();
	m_deviceContext.Reset();
	m_device.Reset();
}

IDWriteFactory* D3DManager::GetDWriteFactory() {
	return m_pDWriteFactory.Get();
}