#pragma once

#include <d3d11_1.h>
#include <wrl/client.h>


class D3DManager {
public:
    // 获取 D3DManager 单例
    static D3DManager& getInstance() {
        static D3DManager instance;
        return instance;
    }

    // 初始化：接受已经创建好的 D3D 设备和上下文
    void initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, IDXGISwapChain* swapChain) {
        m_device = device;
        m_deviceContext = deviceContext;
        m_swapChain = swapChain;
    }

    // 清空渲染目标和深度模板
    void clearRenderTarget(float color[4]);
    void clearDepthStencil(float depth = 1.0f, UINT8 stencil = 0);

    // Present 交换链缓冲区
    void present(bool vsync = false);

    // 获取 D3D 设备和上下文
    ID3D11Device* getDevice() const { return m_device.Get(); }
    ID3D11DeviceContext* getDeviceContext() const { return m_deviceContext.Get(); }
    IDXGISwapChain* getSwapChain() const { return m_swapChain.Get(); }

    // 设置渲染目标视图
    void setRenderTarget(ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView);

    // 释放所有资源
    void cleanup();

    // 禁止拷贝构造与赋值
    D3DManager(const D3DManager&) = delete;
    D3DManager& operator=(const D3DManager&) = delete;

private:
    // 构造与析构
    D3DManager() = default;
    ~D3DManager() { cleanup(); }

    // 使用模板别名(C++11)简化类型名
    template <class T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    // D3D 设备和上下文
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_deviceContext;
    ComPtr<IDXGISwapChain> m_swapChain;

    // 预留接口：支持渲染目标视图和深度模板视图的管理
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    ComPtr<ID3D11DepthStencilView> m_depthStencilView;
};

