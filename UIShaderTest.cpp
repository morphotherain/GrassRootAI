//#include "UIShaderTest.h"
//
//using namespace DirectX;
//
//const D3D11_INPUT_ELEMENT_DESC UIShaderTest::VertexPosColor::inputLayout[3] = {
//	// 位置字段
//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	// 纹理坐标字段
//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	// 纹理索引字段
//	{ "TEXINDEX", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//};
//
//struct TimeConstantBuffer
//{
//	float frameTime;   // 当前帧数
//	float padding[3]; // 添加填充以确保数据对齐
//};
//
//struct MapVertexPosColor {
//	XMFLOAT3 position;
//	XMFLOAT2 texCoord;
//	float texIndex;    // 纹理索引，作为浮点数存储
//};
//
//
//std::vector<MapVertexPosColor> GenerateTestVertices(float x, float y, float deltaX, float deltaY) {
//	std::vector<MapVertexPosColor> vertices;
//
//	vertices.reserve(6); // 每个格子两个三角形，每个三角形3个顶点
//
//
//	// 第一个三角形
//	vertices.push_back({ XMFLOAT3(x, y, -0.0f),             XMFLOAT2(0.0f, 1.0f), 0 });
//	vertices.push_back({ XMFLOAT3(x, (y + deltaY), -0.0f),       XMFLOAT2(0.0f, 0.0f), 0 });
//	vertices.push_back({ XMFLOAT3((x + deltaX), y, -0.0f),       XMFLOAT2(1.0f, 1.0f), 0 });
//
//	// 第二个三角形
//	vertices.push_back({ XMFLOAT3((x + deltaX), y, -0.0f),       XMFLOAT2(1.0f, 1.0f), 0 });
//	vertices.push_back({ XMFLOAT3(x, (y + deltaY), -0.0f),       XMFLOAT2(0.0f, 0.0f), 0 });
//	vertices.push_back({ XMFLOAT3((x + deltaX), (y + deltaY), -0.0f), XMFLOAT2(1.0f, 0.0f), 0 });
//
//
//	return vertices;
//}
//
//
//bool UIShaderTest::Init()
//{
//
//	if (!InitEffect())
//		return false;
//
//	if (!InitResource())
//		return false;
//
//	return false;
//}
//
//void UIShaderTest::OnResize()
//{
//}
//
//void UIShaderTest::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene)
//{
//	// 更新鼠标事件，获取相对偏移量
//	Mouse::State mouseState = mouse.GetState();
//	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
//	m_MouseTracker.Update(mouseState);
//
//	Keyboard::State keyState = keyboard.GetState();
//	m_KeyboardTracker.Update(keyState);
//
//
//
//		
//	// 在鼠标没进入窗口前仍为ABSOLUTE模式
//	if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && mouseState.leftButton == true)
//	{
//		if ((x * 10) < mouseState.x && ((x + deltaX) * 10) > mouseState.x && (1080 - y * 10 - deltaY * 10) < mouseState.y && (1080 - y * 10) > mouseState.y)
//			switchScene = 2;
//	}
//	
//
//}
//
//void UIShaderTest::DrawUI()
//{
//	assert(m_pd3dImmediateContext);
//	assert(m_pSwapChain);
//
//
//
//	// 输入装配阶段的顶点缓冲区设置
//	UINT stride = sizeof(VertexPosColor);	// 跨越字节数
//	UINT offset = 0;						// 起始偏移量
//
//	//按钮绘制
//
//
//
//	m_pd3dImmediateContext->PSSetShader(m_pTestPixelShader.Get(), nullptr, 0);
//
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	HR(m_pd3dImmediateContext->Map(m_pTimeConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
//
//	TimeConstantBuffer* dataPtr = (TimeConstantBuffer*)mappedResource.pData;
//	dataPtr->frameTime = currentTime++;
//	m_pd3dImmediateContext->Unmap(m_pTimeConstantBuffer.Get(), 0);
//
//
//	// 将常量缓冲区绑定到顶点着色器
//	m_pd3dImmediateContext->PSSetConstantBuffers(0, 1, m_pTimeConstantBuffer.GetAddressOf());
//
//
//	m_pd3dImmediateContext->PSSetShaderResources(0, 1, button_textureArraySRV.GetAddressOf()); //绑定纹理
//
//	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, button_m_pVertexBuffer.GetAddressOf(), &stride, &offset);
//
//	m_pd3dImmediateContext->Draw(6, 0);
//
//}
//
//void UIShaderTest::cleanup()
//{
//}
//
//
//
//bool UIShaderTest::InitResource()
//{
//
//	m_ddsLoader.Init(*m_pd3dDevice.GetAddressOf(), *m_pd3dImmediateContext.GetAddressOf());
//
//	std::vector<std::string> textureButtonFileNames = {
//		TexPath
//	};
//
//	m_ddsLoader.InitTex32ArrayFromFiles(textureButtonFileNames, button_textureArraySRV);
//
//	std::vector<MapVertexPosColor> button_vertices = GenerateTestVertices(x, y, deltaX, deltaY);
//	// 设置顶点缓冲区描述
//	D3D11_BUFFER_DESC vbd;
//	ZeroMemory(&vbd, sizeof(vbd));
//	vbd.Usage = D3D11_USAGE_IMMUTABLE;
//	vbd.ByteWidth = sizeof(VertexPosColor) * button_vertices.size(); // 注意这里的变化
//	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vbd.CPUAccessFlags = 0;
//	// 新建顶点缓冲区
//	D3D11_SUBRESOURCE_DATA InitData;
//	ZeroMemory(&InitData, sizeof(InitData));
//	InitData.pSysMem = button_vertices.data();
//	HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, button_m_pVertexBuffer.GetAddressOf()));
//
//	D3D11_BUFFER_DESC cbd;
//	ZeroMemory(&cbd, sizeof(D3D11_BUFFER_DESC));
//	cbd.Usage = D3D11_USAGE_DYNAMIC;
//	cbd.ByteWidth = sizeof(TimeConstantBuffer);
//	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//
//	HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pTimeConstantBuffer.GetAddressOf()));
//
//
//
//	return true;
//}
//
//bool UIShaderTest::InitEffect()
//{	
//
//
//	// Load and create the second pixel shader
//	ComPtr<ID3DBlob> testPsBlob;
//	HR(CreateShaderFromFile(L"HLSL\\test_PS.cso", L"HLSL\\test_PS.hlsl", "PS", "ps_5_0", testPsBlob.ReleaseAndGetAddressOf()));
//	HR(m_pd3dDevice->CreatePixelShader(testPsBlob->GetBufferPointer(), testPsBlob->GetBufferSize(), nullptr, m_pTestPixelShader.GetAddressOf()));
//
//
//
//
//	return true;
//}