
#pragma once
#include "UIText.h"
#include <vector>

// 定义一个结构体来存储文本片段和对应的颜色
struct TextSegment {
    std::wstring text;
    D2D1::ColorF color;
};

class UIRichText : public UIText {
public:
    UIRichText() = default;
    UIRichText(HINSTANCE _hInstance) : UIText(_hInstance) {}
    ~UIRichText() = default;

    // 添加一个文本片段和对应的颜色
    void addTextSegment(const std::wstring& text, const D2D1::ColorF& color);

    // 设置文字是否居中
    void setTextCentered(bool centered);

    // 重写 DrawUI 方法以支持多种颜色和居中
    virtual void DrawUI() override;

private:
    std::vector<TextSegment> segments;
    bool isCentered = false;
};