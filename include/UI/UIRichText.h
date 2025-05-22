
#pragma once
#include "UIText.h"
#include <vector>

enum class TextAlignment {
    Left,
    Center,
    Right
};

struct TextSegment {
    TextSegment(const D2D1::ColorF& color):color(color) {};
    std::wstring text;
    D2D1::ColorF color;
    float fontSize;
    ComPtr<IDWriteTextFormat> textFormat;
    ComPtr<IDWriteTextLayout> textLayout;
    float width; // 存储文本段的宽度
};


class UIRichText : public UIText {
public:
    UIRichText() = default;
    UIRichText(HINSTANCE _hInstance) : UIText(_hInstance) {}
    ~UIRichText() = default;

    // 添加一个文本片段和对应的颜色
    void addTextSegment(const std::wstring& text, const D2D1::ColorF& color, float fontSize);

    // 设置文字是否居中
    void setTextCentered(bool centered);

    // 重写 DrawUI 方法以支持多种颜色和居中
    virtual void DrawUI() override;



    // 设置透明度
    void setTransparency(float transparency) { this->transparency = std::clamp(transparency, 0.0f, 1.0f); }
    
    void clearTextSegments();
private:
    std::vector<TextSegment> segments;
    bool isCentered = false;
    float transparency = 1.0f; // 透明度，范围 0.0 - 1.0
    float maxFontSize = 0.0f;
};