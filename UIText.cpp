#include "UIText.h"

using namespace DirectX;



bool UIText::Init()
{

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	clickFlag = std::make_shared<bool>(false);

	return false;
}

void UIText::OnResize()
{
}

void UIText::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene)
{

}

void UIText::DrawUI()
{

	if (m_pd2dRenderTarget != nullptr)
	{
		m_pd2dRenderTarget->BeginDraw();
		std::wstring textStr = Text;
		m_pd2dRenderTarget->DrawTextW(textStr.c_str(), textStr.size(), m_pTextFormat.Get(),
			D2D1_RECT_F{ x,  y, x+deltaX, y +deltaY}, m_pColorBrush.Get());
		HR(m_pd2dRenderTarget->EndDraw());
	}


}

void UIText::cleanup()
{
}



bool UIText::InitResource()
{

	return true;
}

bool UIText::InitEffect()
{

	return true;
}