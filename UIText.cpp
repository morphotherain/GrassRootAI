#include "UIText.h"

using namespace DirectX;



bool UIText::Init()
{

	m_pd2dRenderTarget = D3DManager::getInstance().GetD2DRenderTarget();
	m_pColorBrush = D3DManager::getInstance().GetColorBrush();
	if(m_pTextFormat == nullptr)
		m_pTextFormat = D3DManager::getInstance().GetTextFormat();

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;


	return false;
}

void UIText::OnResize()
{
}

void UIText::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{

}

void UIText::DrawUI()
{

	if (m_pd2dRenderTarget != nullptr)
	{
		m_pd2dRenderTarget->BeginDraw();
		std::wstring textStr = Text;
		m_pd2dRenderTarget->DrawTextW(textStr.c_str(), static_cast<UINT32>(textStr.size()), m_pTextFormat.Get(),
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


void UIText::switchTextFormat(std::string index)
{
	m_pTextFormat = D3DManager::getInstance().GetTextFormat(index);
}
