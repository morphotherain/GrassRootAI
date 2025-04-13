#include "UIRichText.h"
#include <algorithm>
#include <dwrite.h>

void UIRichText::addTextSegment(const std::wstring& text, const D2D1::ColorF& color) {
    segments.push_back({ text, color });
}

void UIRichText::setTextCentered(bool centered) {
    isCentered = centered;
}

void UIRichText::DrawUI() {
    if (m_pd2dRenderTarget != nullptr) {
        m_pd2dRenderTarget->BeginDraw();

        float currentX = x;
        if (isCentered) {
            // 计算所有文本片段的总宽度
            float totalWidth = 0.0f;
            for (const auto& segment : segments) {
                ComPtr<IDWriteTextLayout> textLayout;
                D3DManager::getInstance().GetDWriteFactory()->CreateTextLayout(
                    segment.text.c_str(),
                    static_cast<UINT32>(segment.text.size()),
                    m_pTextFormat.Get(),
                    10000, 10000,
                    textLayout.GetAddressOf()
                );
                DWRITE_TEXT_METRICS metrics;
                textLayout->GetMetrics(&metrics);
                totalWidth += metrics.width;
            }
            // 计算居中所需的偏移量
            currentX = x + (width - totalWidth) / 2.0f;
        }

        for (const auto& segment : segments) {
            // 创建一个新的颜色刷
            ComPtr<ID2D1SolidColorBrush> colorBrush;
            m_pd2dRenderTarget->CreateSolidColorBrush(segment.color, colorBrush.GetAddressOf());

            m_pd2dRenderTarget->DrawTextW(
                segment.text.c_str(),
                static_cast<UINT32>(segment.text.size()),
                m_pTextFormat.Get(),
                D2D1::RectF(currentX + deltaX, y + deltaY, currentX + width + deltaX, y + height + deltaY),
                colorBrush.Get()
            );

            // 计算当前文本片段的宽度，更新当前 x 坐标
            ComPtr<IDWriteTextLayout> textLayout;
            D3DManager::getInstance().GetDWriteFactory()->CreateTextLayout(
                segment.text.c_str(),
                static_cast<UINT32>(segment.text.size()),
                m_pTextFormat.Get(),
                10000, 10000,
                textLayout.GetAddressOf()
            );
            DWRITE_TEXT_METRICS metrics;
            textLayout->GetMetrics(&metrics);
            currentX += metrics.width;
        }

        HR(m_pd2dRenderTarget->EndDraw());
    }
}