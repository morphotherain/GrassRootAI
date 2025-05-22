#include "UITable.h"
#include "UIRichText.h" // 用于文本渲染

void UITable::setCellText(int row, int col, const std::wstring& text) {
    cells[row][col].text = text;
    if (!cellRichTexts[row][col]) {
        cellRichTexts[row][col] = std::make_unique<UIRichText>();
        cellRichTexts[row][col]->setSize(posX, posY, getColumnWidth(col), getRowHeight(row));
        cellRichTexts[row][col]->setTextCentered(getCellAlignment(row, col) == TextAlignment::Center);
        cellRichTexts[row][col]->Init();
    }
    cellRichTexts[row][col]->clearTextSegments();
    cellRichTexts[row][col]->addTextSegment(text, cells[row][col].color, cells[row][col].fontSize);

}

void UITable::setCellText(int row, int col, const std::wstring& text, float fontSize, const D2D1::ColorF& color, float transparency) {
    cells[row][col].text = text;
    cells[row][col].fontSize = fontSize;
    cells[row][col].color = color;
    cells[row][col].transparency = transparency;
    if (!cellRichTexts[row][col]) {
        cellRichTexts[row][col] = std::make_unique<UIRichText>();
        cellRichTexts[row][col]->setSize(posX, posY, getColumnWidth(col), getRowHeight(row));
        cellRichTexts[row][col]->setTextCentered(getCellAlignment(row, col) == TextAlignment::Center);
        cellRichTexts[row][col]->Init();
    }
    cellRichTexts[row][col]->clearTextSegments();
    cellRichTexts[row][col]->addTextSegment(text, color, fontSize);
}

void UITable::setCellUserData(int row, int col, const std::any& data) {
    cells[row][col].userData = data;
}

void UITable::setRowHeight(int row, float height) {
    rowHeights[row] = height;
}

void UITable::setColumnWidth(int col, float width) {
    columnWidths[col] = width;
}

void UITable::setCellAlignment(int row, int col, TextAlignment alignment) {
    cellAlignments[row][col] = alignment;
}

void UITable::setPosition(float x, float y) {
    posX = x;
    posY = y;
}

void UITable::setTransparency(float transparency) {
    this->transparency = std::clamp(transparency, 0.0f, 1.0f);
}

bool UITable::Init()
{
    int rows = getMaxRows();
    int cols = getMaxCols();

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            if (!cellRichTexts[row][col]) {
                cellRichTexts[row][col] = std::make_unique<UIRichText>();
                cellRichTexts[row][col]->setSize(posX, posY, getColumnWidth(col), getRowHeight(row));
                cellRichTexts[row][col]->setTextCentered(getCellAlignment(row, col) == TextAlignment::Left);
                cellRichTexts[row][col]->Init();
            }
        }
    }

    return true;
}

void UITable::OnResize()
{
}

void UITable::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick) {
    // 更新鼠标状态
    DirectX::Mouse::State mouseState = mouse.GetState();
    mouseTracker.Update(mouseState);

    // 重置点击事件
    lastClickEvent = {};

    // 检测鼠标是否在表格区域内
    float tableWidth = 0.0f;
    for (int col = 0; col < getMaxCols(); ++col) {
        tableWidth += getColumnWidth(col);
    }

    float tableHeight = 0.0f;
    for (int row = 0; row < getMaxRows(); ++row) {
        tableHeight += getRowHeight(row);
    }

    bool isMouseInTable = (mouseState.x >= posX + deltaX && mouseState.x < posX + deltaX + tableWidth &&
        mouseState.y >= posY + deltaY && mouseState.y < posY + deltaY + tableHeight);

    if (isMouseInTable) {
        // 计算鼠标所在的行和列
        float currentY = posY + deltaY;
        int row = -1;
        for (int i = 0; i < getMaxRows(); ++i) {
            float rowHeight = getRowHeight(i);
            if (mouseState.y >= currentY && mouseState.y < currentY + rowHeight) {
                row = i;
                break;
            }
            currentY += rowHeight;
        }

        float currentX = posX + deltaX;
        int col = -1;
        for (int i = 0; i < getMaxCols(); ++i) {
            float colWidth = getColumnWidth(i);
            if (mouseState.x >= currentX && mouseState.x < currentX + colWidth) {
                col = i;
                break;
            }
            currentX += colWidth;
        }

        // 检测鼠标点击
        bool isDoubleClick = false;
        if (mouseTracker.leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED) {
            // 检查是否是双击（时间间隔小于300ms）
            if (tick - lastClickTick < 30 && row == lastClickRow && col == lastClickCol) {
                isDoubleClick = true;
            }
            lastClickTick = tick;
            lastClickRow = row;
            lastClickCol = col;

            // 设置左键点击事件
            lastClickEvent = { row, col, MouseButton::Left, isDoubleClick };
            if (row >= 0 && col >= 0 && cells.find(row) != cells.end() && cells[row].find(col) != cells[row].end()) {
                lastClickEvent.userData = cells[row][col].userData;
            }
        }
        else if (mouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::PRESSED) {
            // 设置右键点击事件
            lastClickEvent = { row, col, MouseButton::Right, false };
            if (row >= 0 && col >= 0 && cells.find(row) != cells.end() && cells[row].find(col) != cells[row].end()) {
                lastClickEvent.userData = cells[row][col].userData;
            }
        }
    }

    int rows = getMaxRows();
    int cols = getMaxCols();

    float currentY = 0;

    for (int row = 0; row < rows; ++row) {
        float currentX = 0;
        float rowHeight = getRowHeight(row);

        for (int col = 0; col < cols; ++col) {
            float colWidth = getColumnWidth(col);

            if (cellRichTexts[row][col]) {
                cellRichTexts[row][col]->setDelta(deltaX + currentX, deltaY + currentY);
            }

            currentX += colWidth;
        }

        currentY += rowHeight;
    }
}

void UITable::DrawUI() {
    int rows = getMaxRows();
    int cols = getMaxCols();

    float currentY = posY;

    for (int row = 0; row < rows; ++row) {
        float currentX = posX;
        float rowHeight = getRowHeight(row);

        for (int col = 0; col < cols; ++col) {
            float colWidth = getColumnWidth(col);

            if (cellRichTexts[row][col]) {
                //cellRichTexts[row][col]->setTransparency(cells[row][col].transparency * this->transparency);
                cellRichTexts[row][col]->DrawUI();
            }

            currentX += colWidth;
        }

        currentY += rowHeight;
    }
}

void UITable::cleanup()
{
}

TableClickEvent UITable::getLastClickEvent() const {
    return lastClickEvent;
}

int UITable::getMaxRows() const {
    int maxRow = 0;
    for (const auto& row : cells) {
        if (row.first >= maxRow) {
            maxRow = row.first + 1;
        }
    }
    return maxRow;
}

int UITable::getMaxCols() const {
    int maxCol = 0;
    for (const auto& row : cells) {
        for (const auto& col : row.second) {
            if (col.first >= maxCol) {
                maxCol = col.first + 1;
            }
        }
    }
    return maxCol;
}

float UITable::getRowHeight(int row) const {
    auto it = rowHeights.find(row);
    if (it != rowHeights.end()) {
        return it->second;
    }
    return 30.0f; // 默认行高
}

float UITable::getColumnWidth(int col) const {
    auto it = columnWidths.find(col);
    if (it != columnWidths.end()) {
        return it->second;
    }
    return 100.0f; // 默认列宽
}

TextAlignment UITable::getCellAlignment(int row, int col) const {
    auto rowIt = cellAlignments.find(row);
    if (rowIt != cellAlignments.end()) {
        auto colIt = rowIt->second.find(col);
        if (colIt != rowIt->second.end()) {
            return colIt->second;
        }
    }
    return TextAlignment::Left; // 默认左对齐
}