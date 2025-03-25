#pragma once
#include <Mouse.h>
#include <functional>
#include "core.h"

class InputHandler {
public:
    // 设置检测区域（矩形）
    void SetDetectionArea(float x, float y, float width, float height);

    // 更新输入状态
    void Update(float dt, const DirectX::Mouse::State& currMouseState);

    // ---------- 鼠标相关功能 ----------
    // 鼠标是否在检测区域内
    bool IsMouseInArea() const;

    // 鼠标是否在区域内按下
    bool IsMousePressedInArea(int mouseButton = 1) const;
    bool IsMouseDragging(int mouseButton = 1) const;

    // 鼠标是否在区域内释放
    bool IsMouseReleasedInArea(int mouseButton = 1) const;

    // 鼠标是否在区域内双击
    bool IsMouseDoubleClickInArea(int mouseButton = 1, float maxInterval = 0.3f) const;

    // 获取鼠标拖拽的偏移量
    void GetDragDelta(int& dx, int& dy) const {
        dx = m_dragDeltaX;
        dy = m_dragDeltaY;
    }

    // ---------- 事件回调 ----------
    // 注册鼠标进入区域事件
    void OnMouseEnter(std::function<void()> callback);

    // 注册鼠标离开区域事件
    void OnMouseLeave(std::function<void()> callback);

private:
    // 检测区域
    struct Rect {
        float x, y, width, height;
    };
    Rect m_detectionArea;

    // 鼠标状态
    DirectX::Mouse::State m_currMouseState, m_prevMouseState;

    // 事件回调
    std::function<void()> m_onMouseEnter, m_onMouseLeave;

    // 双击计时器
    mutable float m_doubleClickTimer[3] = { 0 }; // 0-左键 1-右键 2-中键

    // 拖拽状态
    bool m_isDragging = false;
    int m_dragStartX = 0;
    int m_dragStartY = 0;
    int m_dragDeltaX = 0;
    int m_dragDeltaY = 0;
};