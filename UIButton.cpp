#include "UIButton.h"

using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC UIButton::VertexPosColor::inputLayout[3] = {
	// 位置字段
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 纹理坐标字段
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 纹理索引字段
	{ "TEXINDEX", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};




std::vector<UIButton::MapVertexPosColor> UIButton::GenerateButtonVertices(float _x, float _y, float _deltaX, float _deltaY) {
	_x = _x / 10.0f;
	_y = 1080.0f - _y;
	_y = _y / 10.0f;
	_deltaX = _deltaX / 10.0f;
	_deltaY = -_deltaY / 10.0f;

	std::vector<MapVertexPosColor> vertices;

	vertices.reserve(6); // 每个格子两个三角形，每个三角形3个顶点


	// 第一个三角形
	vertices.push_back({ XMFLOAT3(_x, (_y + _deltaY), -0.0f),             XMFLOAT2(0.0f, 1.0f), 0 });
	vertices.push_back({ XMFLOAT3(_x, (_y), -0.0f),       XMFLOAT2(0.0f, 0.0f), 0 });
	vertices.push_back({ XMFLOAT3((_x + _deltaX), (_y + _deltaY), -0.0f),       XMFLOAT2(1.0f, 1.0f), 0 });

	// 第二个三角形
	vertices.push_back({ XMFLOAT3((_x + _deltaX), (_y + _deltaY), -0.0f),       XMFLOAT2(1.0f, 1.0f), 0 });
	vertices.push_back({ XMFLOAT3(_x, (_y), -0.0f),       XMFLOAT2(0.0f, 0.0f), 0 });
	vertices.push_back({ XMFLOAT3((_x + _deltaX), (_y), -0.0f), XMFLOAT2(1.0f, 0.0f), 0 });


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

void UIButton::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene)
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
			switchScene = 2;
			*clickFlag = true;
	}
	

}

void UIButton::DrawUI()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(VertexPosColor);	// 跨越字节数
	UINT offset = 0;						// 起始偏移量

	//按钮绘制



	//m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);


	m_pd3dImmediateContext->PSSetShaderResources(0, 1, button_textureArraySRV.GetAddressOf()); //绑定纹理

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, button_m_pVertexBuffer.GetAddressOf(), &stride, &offset);

	m_pd3dImmediateContext->Draw(6, 0);

}

void UIButton::cleanup()
{
}



bool UIButton::InitResource()
{

	m_ddsLoader.Init(*m_pd3dDevice.GetAddressOf(), *m_pd3dImmediateContext.GetAddressOf());

	std::vector<std::string> textureButtonFileNames = {
		TexPath
	};

	m_ddsLoader.InitTex32ArrayFromFiles(textureButtonFileNames, button_textureArraySRV);

	std::vector<MapVertexPosColor> button_vertices = GenerateButtonVertices(x, y, deltaX, deltaY);
	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexPosColor) * button_vertices.size(); // 注意这里的变化
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = button_vertices.data();
	HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, button_m_pVertexBuffer.GetAddressOf()));


	return true;
}

bool UIButton::InitEffect()
{	

	return true;
}