#include "InputHandler.h"


bool InputHandler::IsMouseInArea() const {
    auto mousePos = DirectX::XMFLOAT2(
        static_cast<float>(m_currMouseState.x),
        static_cast<float>(m_currMouseState.y)
    );

    return (mousePos.x >= m_detectionArea.x &&
        mousePos.y >= m_detectionArea.y &&
        mousePos.x <= m_detectionArea.x + m_detectionArea.width &&
        mousePos.y <= m_detectionArea.y + m_detectionArea.height);
}

bool InputHandler::IsMousePressedInArea(int mouseButton) const {
    bool inArea = IsMouseInArea();
    bool currDown = (mouseButton == 1) ? m_currMouseState.leftButton :
        (mouseButton == 2) ? m_currMouseState.rightButton :
        m_currMouseState.middleButton;
    bool prevDown = (mouseButton == 1) ? m_prevMouseState.leftButton :
        (mouseButton == 2) ? m_prevMouseState.rightButton :
        m_prevMouseState.middleButton;



    return inArea && currDown && !prevDown;
}

bool InputHandler::IsMouseDragging(int mouseButton) const
{
    return m_isDragging;
}

bool InputHandler::IsMouseReleasedInArea(int mouseButton) const {
    bool inArea = IsMouseInArea();
    bool currDown = (mouseButton == 1) ? m_currMouseState.leftButton :
        (mouseButton == 2) ? m_currMouseState.rightButton :
        m_currMouseState.middleButton;
    bool prevDown = (mouseButton == 1) ? m_prevMouseState.leftButton :
        (mouseButton == 2) ? m_prevMouseState.rightButton :
        m_prevMouseState.middleButton;

    return inArea && !currDown && prevDown;
}

bool InputHandler::IsMouseDoubleClickInArea(int mouseButton, float maxInterval) const {
    bool inArea = IsMouseInArea();
    bool currDown = (mouseButton == 1) ? m_currMouseState.leftButton :
        (mouseButton == 2) ? m_currMouseState.rightButton :
        m_currMouseState.middleButton;
    bool prevDown = (mouseButton == 1) ? m_prevMouseState.leftButton :
        (mouseButton == 2) ? m_prevMouseState.rightButton :
        m_prevMouseState.middleButton;

    if (inArea && currDown && !prevDown) {
        if (m_doubleClickTimer[mouseButton - 1] > 0 &&
            m_doubleClickTimer[mouseButton - 1] <= maxInterval)
        {
            m_doubleClickTimer[mouseButton - 1] = 0;
            return true;
        }
        m_doubleClickTimer[mouseButton - 1] = maxInterval;
    }
    return false;
}

void InputHandler::SetDetectionArea(float x, float y, float width, float height)
{
    m_detectionArea.x = x;
    m_detectionArea.y = y;
    m_detectionArea.width = width;
    m_detectionArea.height = height;
}

void InputHandler::Update(float dt, const DirectX::Mouse::State& currMouseState)
{
    m_prevMouseState = m_currMouseState;
    m_currMouseState = currMouseState;

    if (IsMousePressedInArea() && !m_isDragging) {
        m_isDragging = true;
        m_dragStartX = m_currMouseState.x;
        m_dragStartY = m_currMouseState.y;
    }
    else if (!m_currMouseState.leftButton && m_isDragging) {
        m_isDragging = false;
    }

    if (m_isDragging) {
        m_dragDeltaX = m_currMouseState.x - m_dragStartX;
        m_dragDeltaY = m_currMouseState.y - m_dragStartY;
        m_dragStartX = m_currMouseState.x;
        m_dragStartY = m_currMouseState.y;
    }

    // 更新双击计时器
    for (int i = 0; i < 3; ++i) {
        if (m_doubleClickTimer[i] > 0) {
            m_doubleClickTimer[i] -= dt;
        }
    }

    // 触发鼠标进入/离开事件
    bool isInArea = IsMouseInArea();
    bool wasInArea = (m_prevMouseState.x >= m_detectionArea.x &&
        m_prevMouseState.y >= m_detectionArea.y &&
        m_prevMouseState.x <= m_detectionArea.x + m_detectionArea.width &&
        m_prevMouseState.y <= m_detectionArea.y + m_detectionArea.height);

    if (isInArea && !wasInArea && m_onMouseEnter) {
        m_onMouseEnter();
    }
    if (!isInArea && wasInArea && m_onMouseLeave) {
        m_onMouseLeave();
    }
}

