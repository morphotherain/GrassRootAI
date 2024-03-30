#include "demoScene.h"

using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC demoScene::VertexPosColor::inputLayout[3] = {
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

std::vector<MapVertexPosColor> GenerateChessboardVertices1(int n) {
	float f_n = n / 2.0f;
	std::vector<MapVertexPosColor> vertices;
	std::vector < std::vector<float> >map = {
		{2, 6, 6, 6},
		{0, 3, 1, 1},
		{0, 6, 6, 6},
		{5, 6, 6, 6}
	};
	vertices.reserve(n * n * 6); // 每个格子两个三角形，每个三角形3个顶点


	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {

			// 左下角的点坐标
			float x = j - n / 2;
			float y = n - 1 - i - n / 2; // 使得(0,0)在左上角

			float indexTex = map[i][j];

			// 添加两个三角形来填充这个格子
			// 第一个三角形
			vertices.push_back({ XMFLOAT3(x / f_n, y / f_n, 0.0f),             XMFLOAT2(0.0f, 1.0f), indexTex });
			vertices.push_back({ XMFLOAT3(x / f_n, (y + 1) / f_n, 0.0f),       XMFLOAT2(0.0f, 0.0f), indexTex });
			vertices.push_back({ XMFLOAT3((x + 1) / f_n, y / f_n, 0.0f),       XMFLOAT2(1.0f, 1.0f), indexTex });

			// 第二个三角形
			vertices.push_back({ XMFLOAT3((x + 1) / f_n, y / f_n, 0.0f),       XMFLOAT2(1.0f, 1.0f), indexTex });
			vertices.push_back({ XMFLOAT3(x / f_n, (y + 1) / f_n, 0.0f),       XMFLOAT2(0.0f, 0.0f), indexTex });
			vertices.push_back({ XMFLOAT3((x + 1) / f_n, (y + 1) / f_n, 0.0f), XMFLOAT2(1.0f, 0.0f), indexTex });
		}
	}

	return vertices;
}


bool demoScene::Init()
{

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	return false;
}

void demoScene::OnResize()
{
}

void demoScene::UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard)
{
	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);

	// 获取子类
	auto cam1st = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);
	auto camPos = cam1st->GetPosition();
	float factor = 1.0f / sqrt(abs(camPos.z) + 2.0f); //控制鼠标拖曳速度

	if (m_CameraMode == CameraMode::Free)
	{
		// ******************
		// 自由摄像机的操作
		//

		// 方向移动
		if (keyState.IsKeyDown(Keyboard::W))
		{
			cam1st->MoveY(dt * 6.0f);
		}
		if (keyState.IsKeyDown(Keyboard::S))
		{
			cam1st->MoveY(dt * -6.0f);
		}
		if (keyState.IsKeyDown(Keyboard::A))
		{
			cam1st->MoveX(dt * -6.0f);
		}
		if (keyState.IsKeyDown(Keyboard::D))
		{
			cam1st->MoveX(dt * 6.0f);
		}
		// 在鼠标没进入窗口前仍为ABSOLUTE模式
		if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && mouseState.leftButton == true)
		{
			auto pos = cam1st->GetPosition();
			auto delta_Y = lastMouseState.y - mouseState.y;
			auto delta_X = lastMouseState.x - mouseState.x;

			cam1st->MoveY(-delta_Y * dt * factor);
			cam1st->MoveX(delta_X * dt * factor);
		}
		auto delta_scroll = mouseState.scrollWheelValue - lastMouseState.scrollWheelValue;
		if (delta_scroll > 1.0f)
			cam1st->MoveZ(1.0f);
		if (delta_scroll < -1.0f)
			cam1st->MoveZ(-1.0f);

	}

	// ******************
	// 更新摄像机
	//

	XMFLOAT3 adjustedPos;
	XMStoreFloat3(&adjustedPos, XMVectorClamp(cam1st->GetPositionXM(),
		XMVectorSet(-499.0f, -499.0f, -10.0f, 0.0f), XMVectorSet(499.0f, 999.0f, -3.0f, 0.0f)));
	cam1st->SetPosition(adjustedPos);



	// 退出程序，这里应向窗口发送销毁信息
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Escape))
		SendMessage(m_hMainWnd, WM_DESTROY, 0, 0);
}

void demoScene::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,0,255)
	static float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA = (255,255,255,255)
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), white);
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 假设 camera 是当前场景中的摄影机对象
	DirectX::XMMATRIX viewMatrix = m_pCamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pCamera->GetProjXM();

	// 映射常量缓冲区
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_pd3dImmediateContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(hr))
	{
		MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
		dataPtr->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
		dataPtr->projection = XMMatrixTranspose(projMatrix);

		// 取消映射常量缓冲区
		m_pd3dImmediateContext->Unmap(matrixBuffer.Get(), 0);

		// 将常量缓冲区绑定到顶点着色器
		m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());
	}


	// 对于nxn棋盘格
	int n = 4;
	int verticesPerSquare = 2 * 3; // 每个格子有两个三角形，每个三角形3个顶点
	int totalVertices = n * n * verticesPerSquare;
	// 绘制三角形
	m_pd3dImmediateContext->Draw(totalVertices, 0);
	HR(m_pSwapChain->Present(1, 0));
}

void demoScene::cleanup()
{
}



bool demoScene::InitResource()
{
	auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);
	m_pCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	camera->SetPosition(XMFLOAT3(100.0f, 100.0f, 10.0f));
	camera->SetFrustum(XM_PI / 3, AspectRatio(), 1.0f, 1000.0f);
	camera->LookTo(XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, +0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	camera->SetPosition(XMFLOAT3(1.0f, 1.0f, -10.0f));


	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;  // 使用点采样
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;   // 禁用U方向上的循环
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;   // 禁用V方向上的循环
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;   // 禁用W方向上的循环，对3D纹理有效
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ComPtr<ID3D11SamplerState> pSamplerState;
	HRESULT hr = m_pd3dDevice->CreateSamplerState(&sampDesc, pSamplerState.GetAddressOf());
	if (FAILED(hr))
	{
		// 处理错误
	}

	// 绑定采样器状态到像素着色器
	m_pd3dImmediateContext->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());

	m_ddsLoader.Init(*m_pd3dDevice.GetAddressOf(), *m_pd3dImmediateContext.GetAddressOf());

	std::vector<std::string> textureFileNames = {
		"demoTex\\Line\\colLine.dds",
		"demoTex\\Line\\rowLine.dds",
		"demoTex\\Nodes\\downNode.dds",
		"demoTex\\Nodes\\rightNode.dds",
		"demoTex\\Nodes\\leftNode.dds",
		"demoTex\\Nodes\\UpNode.dds",
		"demoTex\\empty.dds",
	};


	m_ddsLoader.InitTex32ArrayFromFiles(textureFileNames, textureArraySRV);



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
	hr = m_pd3dImmediateContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
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




	std::vector<MapVertexPosColor> vertices = GenerateChessboardVertices1(4);
	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexPosColor) * vertices.size(); // 注意这里的变化
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices.data();
	HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));


	// ******************
	// 给渲染管线各个阶段绑定好所需资源
	//

	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(VertexPosColor);	// 跨越字节数
	UINT offset = 0;						// 起始偏移量

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	// 设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	// 将着色器绑定到渲染管线
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);


	m_pd3dImmediateContext->PSSetShaderResources(0, 1, textureArraySRV.GetAddressOf());
	// ******************
	// 设置调试对象名
	//
	D3D11SetDebugObjectName(m_pVertexLayout.Get(), "VertexPosColorLayout");
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	D3D11SetDebugObjectName(m_pVertexShader.Get(), "Trangle_VS");
	D3D11SetDebugObjectName(m_pPixelShader.Get(), "Trangle_PS");
	return true;
}

bool demoScene::InitEffect()
{
	ComPtr<ID3DBlob> blob;

	// 创建顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\Triangle_VS.cso", L"HLSL\\Triangle_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	// 创建并绑定顶点布局
	HR(m_pd3dDevice->CreateInputLayout(VertexPosColor::inputLayout, ARRAYSIZE(VertexPosColor::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));

	// 创建像素着色器
	HR(CreateShaderFromFile(L"HLSL\\Triangle_PS.cso", L"HLSL\\Triangle_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

	return true;
}
