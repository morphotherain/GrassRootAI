#include "UIRichText.h"
#include <algorithm>
#include <dwrite.h>

void UIRichText::addTextSegment(const std::wstring& text, const D2D1::ColorF& color, float fontSize) {
    TextSegment segment(color);
    segment.text = text;
    segment.color = color;
    segment.fontSize = fontSize;

    // 创建对应字体尺寸的文本格式
    D3DManager::getInstance().GetDWriteFactory()->CreateTextFormat(
        L"Segoe UI", // 字体名称
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"en-US",
        segment.textFormat.GetAddressOf()
    );

    // 使用 IDWriteFactory 创建文本布局
    D3DManager::getInstance().GetDWriteFactory()->CreateTextLayout(
        text.c_str(),
        static_cast<UINT32>(text.size()),
        segment.textFormat.Get(),
        10000, 10000,
        segment.textLayout.GetAddressOf()
    );

    // 获取文本段的宽度
    DWRITE_TEXT_METRICS metrics;
    segment.textLayout->GetMetrics(&metrics);
    segment.width = metrics.width;

    segments.push_back(segment);
    maxFontSize = (std::max)(fontSize, maxFontSize);
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
                totalWidth += segment.width;
            }
            // 计算居中所需的偏移量
            currentX = x + (width - totalWidth) / 2.0f;
        }

        for (const auto& segment : segments) {
            // 应用透明度
            D2D1::ColorF adjustedColor = segment.color;
            adjustedColor.a *= transparency;

            // 创建一个新的颜色刷
            ComPtr<ID2D1SolidColorBrush> colorBrush;
            m_pd2dRenderTarget->CreateSolidColorBrush(adjustedColor, colorBrush.GetAddressOf());

            m_pd2dRenderTarget->DrawTextLayout(
                D2D1::Point2F(currentX + deltaX, y + deltaY + (maxFontSize - segment.fontSize)),
                segment.textLayout.Get(),
                colorBrush.Get()
            );

            // 更新当前 x 坐标
            currentX += segment.width;
        }

        HR(m_pd2dRenderTarget->EndDraw());
    }
}

void UIRichText::clearTextSegments()
{
    segments.clear();
    maxFontSize = 0.0f;
}
