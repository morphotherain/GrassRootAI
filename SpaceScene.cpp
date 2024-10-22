#include "SpaceScene.h"
#include "UIButton.h"
#include "UIShaderTest.h"
#include "UIWindowInfo.h"
#include "UIWindowOverview.h"
#include "UIWindowMap.h"
#include "RenderComponent.h"
#include "DatabaseManager.h"

#include "eveBracketsManager.h"

using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC SpaceScene::VertexPosColor::inputLayout[2] = {
	// 位置字段
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 纹理索引字段
	{ "TEXINDEX", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

SpaceScene::SpaceScene(HINSTANCE _hInstance) : Scene(_hInstance)
{

}

bool SpaceScene::Init()
{

	fs.open(L"C:\\Users\\DottogNoggle\\Desktop\\output.txt", std::fstream::in | std::fstream::out | std::fstream::app);

	m_pSolarSystem = std::make_shared<SolarSystem>(30000001);
	m_pSolarSystem->getDenormalizesBySolarSystemID();

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	for (auto& vertex : vertices3D) {
		auto text = std::make_shared<UIText>();
		text->setSize(0.0f, 0.0f, 350.0f, 350.0f); // 设置文本位置和尺寸
		text->setText(vertex.name); // 设置星域名称文本
		AddUIComponent(text); //添加 UI 组件

		// 存储文本对象以便后续更新
		vertex.text = (text);
	}

	auto text = std::make_shared<UIText>();
	text->setSize(300.0f, 80.0f, 350.0f, 350.0f); // 设置文本位置和尺寸
	text->setText(m_pSolarSystem->m_solarSystem.regionName); // 设置星域名称文本
	AddUIComponent(text); // 添加 UI 组件


	text = std::make_shared<UIText>();
	text->setSize(200.0f, 80.0f, 350.0f, 350.0f); // 设置文本位置和尺寸
	text->setText(m_pSolarSystem->m_solarSystem.constellationName + L" < "); // 设置星域名称文本
	AddUIComponent(text); // 添加 UI 组件


	text = std::make_shared<UIText>();
	text->setSize(100.0f, 80.0f, 350.0f, 350.0f); // 设置文本位置和尺寸
	text->setText(m_pSolarSystem->m_solarSystem.solarSystemName + L" < "); // 设置星域名称文本
	AddUIComponent(text); // 添加 UI 组件

	auto window = std::make_shared<UIWindowInfo>();
	window->setSize(100.0f, 400.0f, 200.0f, 40.0f);
	window->setTypeID(620);
	AddUIComponent(window);

	auto window_overview = std::make_shared<UIWindowOverview>();
	window_overview->setSize(1500.0f, 200.0f, 200.0f, 40.0f);
	AddUIComponent(window_overview);

	auto starmap = std::make_shared<UIWindowMap>();
	starmap->setSize(500.0f, 350.0f, 1120.0f, 657.0f);
	AddUIComponent(starmap);


	for (auto& component : uiComponents) {
		component->setd3dResource(
			*m_pd3dDevice.GetAddressOf(),
			*m_pd3dImmediateContext.GetAddressOf(),
			*m_pSwapChain.GetAddressOf(),
			m_hMainWnd,
			*m_pRenderTargetView.GetAddressOf(),
			*m_pDepthStencilView.GetAddressOf()
		);
		component->setd2dResource(*m_pd2dRenderTarget.GetAddressOf(), *m_pColorBrush.GetAddressOf(), *m_pTextFormat.GetAddressOf());
		component->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		component->Init();
	}

	m_skybox = std::make_shared<UISkyBox>();
	m_skybox->setTex("demoTex\\SpaceScene\\skybox.dds");
	m_skybox->setd3dResource(
		*m_pd3dDevice.GetAddressOf(),
		*m_pd3dImmediateContext.GetAddressOf(),
		*m_pSwapChain.GetAddressOf(),
		m_hMainWnd,
		*m_pRenderTargetView.GetAddressOf(),
		*m_pDepthStencilView.GetAddressOf()
	);
	m_skybox->setd2dResource(*m_pd2dRenderTarget.GetAddressOf(), *m_pColorBrush.GetAddressOf(), *m_pTextFormat.GetAddressOf());
	m_skybox->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
	m_skybox->Init();

	return true;
}

void SpaceScene::OnResize()
{
}

void SpaceScene::UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene)
{
	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);

	// 获取子类
	auto cam3st = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pCamera);
	auto camPos = cam3st->GetPosition();
	float factor = 1.0f / sqrt(abs(camPos.z) + 2.0f); //控制鼠标拖曳速度

	if (m_CameraMode == CameraMode::Free)
	{
		// ******************
		// 自由摄像机的操作
		//
		cam3st->Approach(0.0f);

		// 方向移动
		if (keyState.IsKeyDown(Keyboard::W))
		{
			cam3st->RotateX(0.002f);
		}
		if (keyState.IsKeyDown(Keyboard::S))
		{
			cam3st->RotateX(-0.002f);
		}
		if (keyState.IsKeyDown(Keyboard::A))
		{
			cam3st->RotateY(-0.002f);
		}
		if (keyState.IsKeyDown(Keyboard::D))
		{
			cam3st->RotateY(0.002f);
		}
		if (keyState.IsKeyDown(Keyboard::Q))
		{
			cam3st->Approach(0.2f * (sqrt(cam3st->GetDistance()) / 15.0f));
		}
		if (keyState.IsKeyDown(Keyboard::E))
		{
			cam3st->Approach(-0.2f * (sqrt(cam3st->GetDistance()) / 15.0f));
		}

		if (keyState.IsKeyDown(Keyboard::H))
		{
			cam3st->SetTarget(XMFLOAT3(cam3st->GetTargetPosition().x + 0.1f, cam3st->GetTargetPosition().y, cam3st->GetTargetPosition().z));
		}
		if (keyState.IsKeyDown(Keyboard::K))
		{
			cam3st->SetTarget(XMFLOAT3(cam3st->GetTargetPosition().x - 0.1f, cam3st->GetTargetPosition().y, cam3st->GetTargetPosition().z));
		}

		if (keyState.IsKeyDown(Keyboard::U))
		{
			cam3st->SetTarget(XMFLOAT3(cam3st->GetTargetPosition().x, cam3st->GetTargetPosition().y + 0.1f, cam3st->GetTargetPosition().z));
		}
		if (keyState.IsKeyDown(Keyboard::J))
		{
			cam3st->SetTarget(XMFLOAT3(cam3st->GetTargetPosition().x, cam3st->GetTargetPosition().y - 0.1f, cam3st->GetTargetPosition().z));
		}

		if (keyState.IsKeyDown(Keyboard::Y))
		{
			cam3st->SetTarget(XMFLOAT3(cam3st->GetTargetPosition().x, cam3st->GetTargetPosition().y, cam3st->GetTargetPosition().z + 0.1f));
		}
		if (keyState.IsKeyDown(Keyboard::I))
		{
			cam3st->SetTarget(XMFLOAT3(cam3st->GetTargetPosition().x, cam3st->GetTargetPosition().y, cam3st->GetTargetPosition().z - 0.1f));
		}
	}

	for (auto& component : uiComponents) {
		component->UpdateUI(dt, mouse, keyboard, switchScene);
	}

	// ******************
	// 更新摄像机
	//

	tick++;

	// 退出程序，这里应向窗口发送销毁信息
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Escape))
		SendMessage(m_hMainWnd, WM_DESTROY, 0, 0);
}

// 将三维坐标转换为标准化设备坐标(NDC)范围的函数
XMFLOAT2 Convert3DToNDC(const XMFLOAT3& worldPos, const XMMATRIX& viewMatrix, const XMMATRIX& projMatrix)
{
	// 将三维世界坐标转换为裁剪空间
	XMVECTOR worldPosition = XMLoadFloat3(&worldPos);
	XMVECTOR clipPosition = XMVector3TransformCoord(worldPosition, viewMatrix * projMatrix);

	// 透视除法，得到标准化设备坐标(NDC)
	XMFLOAT3 ndcPos;
	XMStoreFloat3(&ndcPos, clipPosition);

	// 返回的ndcPos.x 和 ndcPos.y 直接是 NDC 范围 [-1, 1]
	return XMFLOAT2(ndcPos.x, ndcPos.y);
}


void SpaceScene::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);


	float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pd3dImmediateContext->OMSetBlendState(m_pBlendState.Get(), blendFactor, 0xffffffff);

	static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,0,255)
	static float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA = (255,255,255,255)
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), black);
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	m_skybox->DrawUI();

	// 假设 camera 是当前场景中的摄影机对象
	DirectX::XMMATRIX viewMatrix = m_pCamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pCamera->GetProjXM();

	// 映射常量缓冲区
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_pd3dImmediateContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(hr))
	{
		MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
		dataPtr->model = XMMatrixTranspose(XMMatrixIdentity());
		dataPtr->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
		dataPtr->projection = XMMatrixTranspose(projMatrix);
		dataPtr->TexIndex = 0;

		// 取消映射常量缓冲区
		m_pd3dImmediateContext->Unmap(matrixBuffer.Get(), 0);

		// 将常量缓冲区绑定到顶点着色器
		m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());
	}

	vertices.clear();

	for (auto& vertex3D : vertices3D)
	{
		// 将每个三维坐标转换为 NDC 坐标
		XMFLOAT2 ndcCoord = Convert3DToNDC(vertex3D.pos, viewMatrix, projMatrix);

		// 创建新的顶点，设置其位置为 NDC 坐标，并保持纹理索引
		MapVertexPosColor vertex;
		vertex.position = XMFLOAT2(ndcCoord.x, ndcCoord.y); // 将 NDC 坐标设置为顶点位置
		vertex.texIndex = vertex3D.texIndex;   // 保留原始的纹理索引

		vertex3D.text->setSize((ndcCoord.x + 1.0f)/2.0f * 1920.0f+20.0f, (-ndcCoord.y + 1.0f) / 2.0f * 1080.0f+3.0f, 350.0f, 350.0f);

		// 将转换后的顶点加入目标顶点数组
		vertices.push_back(vertex);
	}
	// 映射并更新顶点缓冲区
	D3D11_MAPPED_SUBRESOURCE mappedResourceVertex;
	HR(m_pd3dImmediateContext->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceVertex));
	
	// 将转换后的顶点数据复制到映射的缓冲区中
// 检查缓冲区大小与数据一致性
	if (mappedResourceVertex.pData != nullptr)
	{
		// 确保数据正确地拷贝到映射缓冲区中
		memcpy(mappedResourceVertex.pData, vertices.data(), sizeof(MapVertexPosColor) * vertices.size());
	}

	// 取消映射顶点缓冲区
	m_pd3dImmediateContext->Unmap(m_pVertexBuffer.Get(), 0);
	


	m_pd3dImmediateContext->PSSetShaderResources(0, 1, textureArraySRV.GetAddressOf()); //绑定纹理
	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(VertexPosColor);	// 跨越字节数
	UINT offset = 0;						// 起始偏移量

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());

	m_pd3dImmediateContext->VSSetShader(m_pVertexShaderBillboard.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(m_pPixelShaderBillboard.Get(), nullptr, 0);
	m_pd3dImmediateContext->GSSetShader(m_pGeometryShader.Get(), nullptr, 0); // 绑定几何着色器

	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	m_pd3dImmediateContext->Draw(vertices.size(), 0);

	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->GSSetShader(nullptr, nullptr, 0); // 绑定几何着色器

	for (auto& component : uiComponents) {
		component->DrawUI();
	}

	HR(m_pSwapChain->Present(1, 0));
}

void SpaceScene::cleanup()
{
	fs.close();
}

bool SpaceScene::InitResource()
{
	tick = 0;

	auto camera = std::shared_ptr<ThirdPersonCamera>(new ThirdPersonCamera);
	m_pCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	camera->SetFrustum(XM_PI / 3, AspectRatio(), 1.0f, 1000.0f);
	camera->SetTarget(XMFLOAT3(0.0f, 0.0f, 1.0f));
	camera->SetDistance(10.0f);
	camera->SetDistanceMinMax(1.0f, 40.0f);
	camera->Approach(-0.00f);

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

	D3D11_BLEND_DESC blendDesc = { 0 };
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// 创建混合状态对象
	hr = m_pd3dDevice->CreateBlendState(&blendDesc, m_pBlendState.GetAddressOf());


	// 绑定采样器状态到像素着色器
	m_pd3dImmediateContext->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());

	m_ddsLoader.Init(*m_pd3dDevice.GetAddressOf(), *m_pd3dImmediateContext.GetAddressOf());

	std::vector<std::string> textureFileNames = eveBracketsManager::getInstance()->getAllDdsPaths();

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
	auto cam1st = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pCamera);

	// 映射常量缓冲区，确保成功后...
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->model = XMMatrixTranspose(XMMatrixIdentity());
	dataPtr->view = XMMatrixTranspose(cam1st->GetViewXM()); // 确保矩阵是列主序以适配HLSL默认
	dataPtr->projection = XMMatrixTranspose(cam1st->GetProjXM());
	dataPtr->TexIndex = 0;

	m_pd3dImmediateContext->Unmap(matrixBuffer.Get(), 0);

	// 设置顶点着色器中的常量缓冲区
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());

	for (auto p_denormalize : m_pSolarSystem->m_denormalizes) {
		double factor = 1000;
		float x = p_denormalize->x / factor;
		float y = p_denormalize->y / factor;
		float z = p_denormalize->z / factor;
		Vertex3DPosIndex temp = { {x,y,z},(p_denormalize->bracketID),p_denormalize->name };
		if(abs(temp.texIndex - 1.0 ) > 0.5f && abs(temp.texIndex - 2.0) > 0.5f && abs(temp.texIndex - 4.0) > 0.5f)
			vertices3D.push_back(temp);
	}
	//vertices3D = {
	//	{{0.0f,0.0f,0.0f},0.0f, L"恒星"},
	//	{{0.0f,1.0f,0.0f},1.0f, L"行星"},
	//	{{1.0f,0.0f,0.0f},2.0f, L"卫星"},
	//	{{0.0f,0.0f,1.0f},3.0f, L"小行星带"},
	//	{{0.0f,0.0f,-1.0f},4.0f, L"星门"},
	//	{{0.0f,-1.0f,0.0f},5.0f, L"空间站"},
	//	{{-1.0f,0.0f,0.0f},5.0f, L"空间站"},
	//};



	vertices = {
		{{0.0f,0.0f},0.0f }
	};

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(VertexPosColor) * vertices3D.size(); // 注意这里的变化
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices.data();
	HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

	// ******************
	// 给渲染管线各个阶段绑定好所需资源
	//

	// 设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	// 将着色器绑定到渲染管线
	m_pd3dImmediateContext->VSSetShader(m_pVertexShaderBillboard.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(m_pPixelShaderBillboard.Get(), nullptr, 0);

	return true;
}

bool SpaceScene::InitEffect()
{
	ComPtr<ID3DBlob> blob;

	// 创建顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\Space\\Billboard_VS.cso", L"HLSL\\Space\\Billboard_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShaderBillboard.GetAddressOf()));

	// 创建并绑定顶点布局
	HR(m_pd3dDevice->CreateInputLayout(VertexPosColor::inputLayout, ARRAYSIZE(VertexPosColor::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));

	// 创建像素着色器
	HR(CreateShaderFromFile(L"HLSL\\Space\\Billboard_PS.cso", L"HLSL\\Space\\Billboard_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShaderBillboard.GetAddressOf()));
	
	// 创建几何着色器
	HR(CreateShaderFromFile(L"HLSL\\Space\\Billboard_GS.cso", L"HLSL\\Space\\Billboard_GS.hlsl", "GS", "gs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pGeometryShader.GetAddressOf()));

	return true;
}