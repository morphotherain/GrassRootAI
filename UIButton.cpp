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

// 定义常量缓冲区的结构
struct MatrixBufferType
{
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

struct MapVertexPosColor {
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
	float texIndex;    // 纹理索引，作为浮点数存储
};


std::vector<MapVertexPosColor> GenerateButtonVertices(float x, float y, float deltaX, float deltaY) {
	std::vector<MapVertexPosColor> vertices;

	vertices.reserve(6); // 每个格子两个三角形，每个三角形3个顶点


	// 第一个三角形
	vertices.push_back({ XMFLOAT3(x, y, -0.0f),             XMFLOAT2(0.0f, 1.0f), 0 });
	vertices.push_back({ XMFLOAT3(x, (y + deltaY), -0.0f),       XMFLOAT2(0.0f, 0.0f), 0 });
	vertices.push_back({ XMFLOAT3((x + deltaX), y, -0.0f),       XMFLOAT2(1.0f, 1.0f), 0 });

	// 第二个三角形
	vertices.push_back({ XMFLOAT3((x + deltaX), y, -0.0f),       XMFLOAT2(1.0f, 1.0f), 0 });
	vertices.push_back({ XMFLOAT3(x, (y + deltaY), -0.0f),       XMFLOAT2(0.0f, 0.0f), 0 });
	vertices.push_back({ XMFLOAT3((x + deltaX), (y + deltaY), -0.0f), XMFLOAT2(1.0f, 0.0f), 0 });


	return vertices;
}


bool UIButton::Init()
{

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

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


	if (m_CameraMode == CameraMode::Free)
	{
		// ******************
		// 自由摄像机的操作
		//

		// 方向移动
		if (keyState.IsKeyDown(Keyboard::W))
		{
			
		}
		if (keyState.IsKeyDown(Keyboard::S))
		{
			
		}
		if (keyState.IsKeyDown(Keyboard::A))
		{
			
		}
		if (keyState.IsKeyDown(Keyboard::D))
		{
			
		}
		// 在鼠标没进入窗口前仍为ABSOLUTE模式
		if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && mouseState.leftButton == true)
		{
			if (80 < mouseState.x && 220 > mouseState.x && 680 < mouseState.y && 715 > mouseState.y)
				switchScene = 2;
		}
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
		"demoTex\\MainScene\\button1.dds"
	};

	m_ddsLoader.InitTex32ArrayFromFiles(textureButtonFileNames, button_textureArraySRV);


	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// 使用设备创建缓冲区
	m_pd3dDevice->CreateBuffer(&matrixBufferDesc, nullptr, matrixBuffer.GetAddressOf());

	// 更新常量缓冲区
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	HRESULT hr = m_pd3dImmediateContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	// 确保检查hr的值...

	// 获取子类
	auto cam1st = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);

	// 映射常量缓冲区，确保成功后...
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->view = XMMatrixTranspose(cam1st->GetViewXM()); // 确保矩阵是列主序以适配HLSL默认
	dataPtr->projection = XMMatrixTranspose(cam1st->GetProjXM());


	m_pd3dImmediateContext->Unmap(matrixBuffer.Get(), 0);

	// 设置顶点着色器中的常量缓冲区
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());

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