#include "UIButton.h"

using namespace DirectX;

std::vector<PosTexIndex> UIButton::GenerateButtonVertices(float _x, float _y, float _deltaX, float _deltaY) {

	std::vector<PosTexIndex> vertices;

	vertices.reserve(6); // 每个格子两个三角形，每个三角形3个顶点

	float TexID = 2;

	// 第一个三角形
	vertices.push_back({ XMFLOAT3(_x, (_y + _deltaY), -0.0f),             XMFLOAT2(0.0f, 1.0f), TexID });
	vertices.push_back({ XMFLOAT3(_x, (_y), -0.0f),       XMFLOAT2(0.0f, 0.0f), TexID });
	vertices.push_back({ XMFLOAT3((_x + _deltaX), (_y + _deltaY), -0.0f),       XMFLOAT2(1.0f, 1.0f), TexID });

	// 第二个三角形
	vertices.push_back({ XMFLOAT3((_x + _deltaX), (_y + _deltaY), -0.0f),       XMFLOAT2(1.0f, 1.0f), TexID });
	vertices.push_back({ XMFLOAT3(_x, (_y), -0.0f),       XMFLOAT2(0.0f, 0.0f), TexID });
	vertices.push_back({ XMFLOAT3((_x + _deltaX), (_y), -0.0f), XMFLOAT2(1.0f, 0.0f), TexID });


	return vertices;
}


bool UIButton::Init()
{

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	clickFlag = std::make_shared<bool>(false);
	return false;
}

void UIButton::OnResize()
{
}

void UIButton::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);
		
	// 在鼠标没进入窗口前仍为ABSOLUTE模式
	if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && mouseState.leftButton == true)
	{
		if (x < mouseState.x && (x + deltaX) > mouseState.x && (y + deltaY) > mouseState.y && y < mouseState.y)
			*clickFlag = true;
	}
	

}

void UIButton::DrawUI()
{
	// 假设 camera 是当前场景中的摄影机对象
	DirectX::XMMATRIX viewMatrix = m_pUICamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pUICamera->GetProjXM();


	XMMATRIX windowModel = XMMatrixTranslation(x, y, 0.0f);

	ConstantMVPIndex* dataPtr = m_effect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtr->model = XMMatrixTranspose(windowModel);
	dataPtr->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
	dataPtr->projection = XMMatrixTranspose(projMatrix);
	dataPtr->TexIndex = 0;
	m_effect->getConstantBuffer<ConstantMVPIndex>()->Unmap();

	m_effect->apply();
	if (text != nullptr) {
		text->DrawUI();
	}

}

void UIButton::cleanup()
{
}



bool UIButton::InitResource()
{
	 
	std::vector<std::string> textureButtonFileNames = {
		TexPath
	};
	m_effect = std::make_shared<Effect>();

	m_effect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	m_effect->getVertexBuffer<PosTexIndex>()->setVertices(GenerateButtonVertices(0, 0, deltaX, deltaY));
	m_effect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	m_effect->addTextures(textureButtonFileNames);
	m_effect->addConstantBuffer<ConstantMVPIndex>();
	m_effect->Init();

	auto UIcamera = std::make_shared<OrthographicCamera>();
	m_pUICamera = UIcamera;

	// 设置正交投影参数
	float left = 0.0f;
	float right = static_cast<float>(m_ClientWidth);
	float bottom = static_cast<float>(m_ClientHeight);
	float top = 0.0f;
	float nearZ = 0.0f;
	float farZ = 1.0f;
	UIcamera->SetOrthographic(left, right, bottom, top, nearZ, farZ);

	return true;
}

bool UIButton::InitEffect()
{	
	return true;
}

void UIButton::setText(std::wstring _text)
{
	text = std::make_shared<UIText>();
	text->setText(_text);
	text->setSize(x+10.0f,y+2.0f,200.0f,200.0f);
	text->Init();
}
