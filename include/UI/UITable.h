#pragma once
#include "UIBase.h"
#include <vector>
#include <string>
#include "UIRichText.h"
#include <memory>
#include <unordered_map>

enum class MouseButton {
    None,
    Left,
    Right
};

struct TableCell {
    std::wstring text;
    float fontSize = 12.0f;
    D2D1::ColorF color = D2D1::ColorF(D2D1::ColorF::White);
    float transparency = 1.0f;
    std::any userData; // 存储任意用户数据（如orderID）
};

struct TableClickEvent {
    int row = -1;
    int col = -1;
    MouseButton button = MouseButton::None;
    bool isDoubleClick = false;
    std::any userData; // 存储单元格的userData

    bool isValid() const { return row >= 0 && col >= 0; }
};


class UITable : public UIBase {
public:
    UITable() = default;
    UITable(HINSTANCE _hInstance) : UIBase(_hInstance) {}
    ~UITable() = default;

    // 设置单元格文本
    void setCellText(int row, int col, const std::wstring& text);
    void setCellText(int row, int col, const std::wstring& text, float fontSize, const D2D1::ColorF& color, float transparency = 1.0f);

    void setCellUserData(int row, int col, const std::any& data);

    // 设置行高和列宽
    void setRowHeight(int row, float height);
    void setColumnWidth(int col, float width);

    // 设置单元格对齐方式
    void setCellAlignment(int row, int col, TextAlignment alignment);

    // 重写UI方法
    virtual bool Init();
    virtual void OnResize();
    virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick) override;
    virtual void DrawUI() override;
    virtual void cleanup();

    // 获取点击事件（返回上一帧的点击事件）
    TableClickEvent getLastClickEvent() const;

    // 设置表格位置和透明度
    void setPosition(float x, float y);
    void setTransparency(float transparency);

private:
    std::unordered_map<int, std::unordered_map<int, TableCell>> cells;
    std::unordered_map<int, float> rowHeights;
    std::unordered_map<int, float> columnWidths;
    std::unordered_map<int, std::unordered_map<int, std::unique_ptr<UIRichText>>> cellRichTexts;
    std::unordered_map<int, std::unordered_map<int, TextAlignment>> cellAlignments;

    float posX = 0.0f;
    float posY = 0.0f;
    float transparency = 1.0f;

    TableClickEvent lastClickEvent; // 存储上一帧的点击事件
    DirectX::Mouse::ButtonStateTracker mouseTracker; // 鼠标状态追踪器
    UINT lastClickTick = 0; // 上次点击的tick（用于双击检测）
    int lastClickRow = -1;
    int lastClickCol = -1;

    // 获取表格的最大行数和列数
    int getMaxRows() const;
    int getMaxCols() const;

    // 获取行高和列宽，如果未设置则使用默认值
    float getRowHeight(int row) const;
    float getColumnWidth(int col) const;

    // 获取单元格对齐方式，如果未设置则使用默认值
    TextAlignment getCellAlignment(int row, int col) const;
};