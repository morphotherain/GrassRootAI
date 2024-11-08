#include "UIButton.h"

using namespace DirectX;

//const D3D11_INPUT_ELEMENT_DESC UIButton::VertexPosColor::inputLayout[3] = {
//	// 位置字段
//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	// 纹理坐标字段
//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	// 纹理索引字段
//	{ "TEXINDEX", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//};
//



std::vector<PosTexIndex> UIButton::GenerateButtonVertices(float _x, float _y, float _deltaX, float _deltaY) {
	_x = _x / 10.0f;
	_y = 1080.0f - _y;
	_y = _y / 10.0f;
	_deltaX = _deltaX / 10.0f;
	_deltaY = -_deltaY / 10.0f;

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
			//switchScene = 2;
			*clickFlag = true;
	}
	

}

void UIButton::DrawUI()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	m_effect->apply();

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
	m_effect->getVertexBuffer<PosTexIndex>()->setVertices(GenerateButtonVertices(x, y, deltaX, deltaY));
	m_effect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	m_effect->addTextures(textureButtonFileNames);
	m_effect->Init();

	return true;
}

bool UIButton::InitEffect()
{	
	return true;
}