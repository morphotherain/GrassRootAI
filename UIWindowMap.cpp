#include "UIWindowMap.h"
#include <random>
#include <DirectXMath.h>
#include <algorithm>  
#include <utility>    // 也包含 std::min
#include <unordered_map>

using namespace DirectX;

// 点的顶点布局描述
const D3D11_INPUT_ELEMENT_DESC UIWindowMap::PointVertexPosColor::inputLayout[3] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },            // pos
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },           // color
	{ "LUMINOSITY", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },               // luminosity
};

// 线条的顶点布局描述
const D3D11_INPUT_ELEMENT_DESC UIWindowMap::LineVertexPosColor::inputLayout[2] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },            // pos
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },           // color
};

UIWindowMap::UIWindowMap() : UIWindow()
{

}

bool UIWindowMap::Init()
{

	UIWindow::windowTitle = L"星图";

	InitWindowComponent();

	if (!InitMap())
		return false;

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	double factor = 40000000000000000.0; // 用于坐标缩放

	for (const auto& region : m_regions)
	{
		// 计算缩放后的坐标
		DirectX::XMFLOAT3 scaledPos = DirectX::XMFLOAT3(
			static_cast<float>(region.x / factor),
			static_cast<float>(region.y / factor),
			static_cast<float>(region.z / factor)
		);

		// 创建并配置文本组件
		auto text = std::make_shared<UIText>();
		text->setSize(scaledPos.x , scaledPos.y, 350.0f, 350.0f); // 设置文本位置和尺寸
		text->setText(region.regionName); // 设置星域名称文本
		AddUIComponent(text); // 假设 AddUIComponent 是用于添加 UI 组件的方法

		// 存储文本对象以便后续更新
		regionTexts.push_back(text);
	}

	for (auto& component : childComponents) {
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


	return false;
}

void UIWindowMap::OnResize()
{
}

void UIWindowMap::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene)
{
	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);

	//auto cam1st = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);
	auto cam3st = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pCamera);


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
		cam3st->RotateY(0.002f);
	}
	if (keyState.IsKeyDown(Keyboard::D))
	{
		cam3st->RotateY(-0.002f);
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
		cam3st->SetTarget(XMFLOAT3(cam3st->GetTargetPosition().x , cam3st->GetTargetPosition().y + 0.1f, cam3st->GetTargetPosition().z));
	}
	if (keyState.IsKeyDown(Keyboard::J))
	{
		cam3st->SetTarget(XMFLOAT3(cam3st->GetTargetPosition().x , cam3st->GetTargetPosition().y - 0.1f, cam3st->GetTargetPosition().z));
	}

	if (keyState.IsKeyDown(Keyboard::Y))
	{
		cam3st->SetTarget(XMFLOAT3(cam3st->GetTargetPosition().x , cam3st->GetTargetPosition().y, cam3st->GetTargetPosition().z + 0.1f));
	}
	if (keyState.IsKeyDown(Keyboard::I))
	{
		cam3st->SetTarget(XMFLOAT3(cam3st->GetTargetPosition().x, cam3st->GetTargetPosition().y, cam3st->GetTargetPosition().z - 0.1f));
	}
	cam3st->RotateY(0.0002f);


	// 在鼠标没进入窗口前仍为ABSOLUTE模式
	if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && mouseState.leftButton == true)
	{

	}


}

void UIWindowMap::DrawUI()
{
	for (auto& component : childComponents) {
		component->DrawUI();
	}
	// 1. 保存当前视口
	UINT numViewports = 1;
	D3D11_VIEWPORT originalViewport;
	m_pd3dImmediateContext->RSGetViewports(&numViewports, &originalViewport);

	// 2. 设置新的视口，留出左边 40 像素
	D3D11_VIEWPORT adjustedViewport = originalViewport;
	adjustedViewport.TopLeftX = x;
	adjustedViewport.TopLeftY = y + TitleHeight;
	adjustedViewport.Width = deltaX;
	adjustedViewport.Height = deltaY - TitleHeight;
	m_pd3dImmediateContext->RSSetViewports(1, &adjustedViewport);


	//应用混合状态
	float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pd3dImmediateContext->OMSetBlendState(m_pBlendState.Get(), blendFactor, 0xffffffff);

	//static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,0,255)
	//static float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA = (255,255,255,255)
	//m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), black);
	//m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);



	// 假设 camera 是当前场景中的摄影机对象
	DirectX::XMMATRIX viewMatrix = m_pCamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pCamera->GetProjXM();

	// 获取摄像机的世界位置
	DirectX::XMVECTOR cameraPosition = m_pCamera->GetPositionXM(); // 假设 GetPositionXM() 返回摄像机位置的 XMVECTOR

	double factor = 40000000000000000.0; // 用于坐标缩放


	// 遍历每个星域名称文本并更新位置
	for (size_t i = 0; i < m_regions.size(); ++i)
	{
		// 计算缩放后的世界坐标
		DirectX::XMFLOAT3 worldPos(
			static_cast<float>(m_regions[i].x / factor),
			static_cast<float>(m_regions[i].y / factor),
			static_cast<float>(m_regions[i].z / factor)
		);

		// 将世界坐标转换为屏幕坐标
		DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&worldPos);
		pos = DirectX::XMVector3TransformCoord(pos, viewMatrix * projMatrix);

		// 将归一化设备坐标映射到屏幕坐标
		DirectX::XMFLOAT2 screenPos;
		screenPos.x = (pos.m128_f32[0] + 1.0f) * 0.5f * adjustedViewport.Width + adjustedViewport.TopLeftX;  // 假设 m_screenWidth 是屏幕宽度

		if (screenPos.x < adjustedViewport.TopLeftX || screenPos.x > adjustedViewport.TopLeftX + adjustedViewport.Width) screenPos.x = -10000.0f;

		screenPos.y = (1.0f - pos.m128_f32[1]) * 0.5f * adjustedViewport.Height + adjustedViewport.TopLeftY; // 假设 m_screenHeight 是屏幕高度

		if (screenPos.y < adjustedViewport.TopLeftY || screenPos.y > adjustedViewport.TopLeftY + adjustedViewport.Height) screenPos.y = -10000.0f;

		// 更新文本位置
		regionTexts[i]->setSize(screenPos.x, screenPos.y , 350.0f, 350.0f);
	}

	// 映射常量缓冲区
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_pd3dImmediateContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(hr))
	{
		MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
		dataPtr->model = XMMatrixTranspose(XMMatrixIdentity());
		dataPtr->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
		dataPtr->projection = XMMatrixTranspose(projMatrix);

		// 将摄像机位置存储到常量缓冲区
		DirectX::XMStoreFloat3(&dataPtr->cameraPosition, cameraPosition); // 将摄像机位置存储到常量缓冲区

		// 取消映射常量缓冲区
		m_pd3dImmediateContext->Unmap(matrixBuffer.Get(), 0);

		// 将常量缓冲区绑定到顶点着色器
		m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());
	}



	// 绘制连接线（线条）
	UINT stride = sizeof(LineVertexPosColor);
	UINT offset = 0;
	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBufferLines.GetAddressOf(), &stride, &offset);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayoutLines.Get());
	m_pd3dImmediateContext->VSSetShader(m_pVertexShaderLines.Get(), nullptr, 0); // 线的顶点着色器
	m_pd3dImmediateContext->PSSetShader(m_pPixelShaderLines.Get(), nullptr, 0); // 线的像素着色器
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	m_pd3dImmediateContext->Draw(numLinePoint, 0);

	int temp = 0;
	// 绘制星系（点）
	stride = sizeof(PointVertexPosColor);
	offset = 0;
	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBufferPoints.GetAddressOf(), &stride, &offset); //这里在图形调试时始终会报错
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayoutPoints.Get());     //!!!!!!!
	m_pd3dImmediateContext->VSSetShader(m_pVertexShaderPoints.Get(), nullptr, 0); // 点的顶点着色器
	m_pd3dImmediateContext->PSSetShader(m_pPixelShaderPoints.Get(), nullptr, 0); // 点的像素着色器
	m_pd3dImmediateContext->GSSetShader(m_pGeometryShader.Get(), nullptr, 0); // 绑定几何着色器
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_pd3dImmediateContext->Draw(numPoint, 0); // 使用实例化渲染
	m_pd3dImmediateContext->GSSetShader(nullptr, nullptr, 0);  // 空置几何着色器


	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0); // 顶点着色器
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0); // 像素着色器
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->RSSetViewports(1, &originalViewport);


}

void UIWindowMap::cleanup()
{
}




// 生成随机浮点数的辅助函数
float RandomFloat(float min, float max)
{
	static std::default_random_engine engine(static_cast<unsigned int>(time(0)));
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(engine);
}

// 计算两点之间的欧几里得距离
float CalculateDistance(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2)
{
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;
	float dz = p1.z - p2.z;
	return sqrt(dx * dx + dy * dy + dz * dz);
}

float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

// 生成随机顶点和线条
void GenerateRandomVerticesAndLines(
	std::vector<UIWindowMap::PointVertexPosColor>& verticesPoints,
	std::vector<UIWindowMap::LineVertexPosColor>& verticesLines,
	int numPoints = 1000, std::vector<SolarSystemData>* solarSystem = nullptr)
{
	// 清空数组
	verticesPoints.clear();
	verticesLines.clear();

	if (solarSystem != nullptr)
	{
		numPoints = solarSystem->size();
	}


	for (int i = 0; i < numPoints; ++i)
	{
		UIWindowMap::PointVertexPosColor point;
		if (solarSystem != nullptr) {
			double factor = 40000000000000000;
			point.pos = DirectX::XMFLOAT3((*solarSystem)[i].x/ factor, (*solarSystem)[i].y/ factor, (*solarSystem)[i].z / factor);
			point.luminosity = (*solarSystem)[i].luminosity>2.0f?2.0f:(*solarSystem)[i].luminosity; // 调整亮度范围
			point.luminosity = point.luminosity * 3.0f;
		}
		else
		{
			// 设置中心分布的概率
			float distributionType = RandomFloat(0.0f, 1.0f);

			if (distributionType < 0.4f) // 20% 的概率生成银心分布
			{
				// 银心分布参数
				float coreDensityFactor = 5.0f;  // 控制中心密集程度
				float maxRadius = 20.0f;         // 最大影响范围
				float a = 10.0f;                 // X方向的半径
				float b = 0.8f;                  // Y方向的半径，较小，形成扁平化
				float c = 10.0f;                 // Z方向的半径

				// 生成均匀分布在椭球内部的随机点
				float u = RandomFloat(0.05f, 1.0f); // 随机半径因子
				float v = RandomFloat(0.0f, 1.0f); // 控制方向的随机因子
				float theta = RandomFloat(0.0f, DirectX::XM_2PI); // 随机旋转角度

				// 使用随机方向矢量生成球内点
				float r = maxRadius * (u * u); // 半径生成，确保均匀体积分布
				float sinTheta = sin(theta);
				float cosTheta = cos(theta);

				// 随机方向生成，确保椭球内均匀分布
				float x = r * a * sinTheta * sqrt(1 - v * v); // X方向的坐标
				float y = r * b * v;                          // Y方向的坐标，缩小形成扁平效果
				float z = r * c * cosTheta * sqrt(1 - v * v); // Z方向的坐标

				// 计算密度衰减，模拟中心密集和外部稀疏
				float decay = 1.0f / (1.0f + r / coreDensityFactor);

				// 根据距离进行 y 轴缩放因子线性插值
				float yFactor = lerp(2.0f, 0.5f, r / maxRadius); // 距离越大，因子越接近1.0

				// 赋值给点的坐标，应用衰减
				point.pos = DirectX::XMFLOAT3(x * decay, (y * decay - 0.5f) * yFactor, z * decay);

			}
			else
			{

				// 银河系参数
				float a = 30.0f;     // 控制旋臂分布的总体大小
				float b = 2.0f;      // 控制银河系的厚度
				int numArms = 2;     // 旋臂数量

				// 控制螺旋的松紧和星体分布
				float spiralTightness = 4.5f; // 控制螺旋的松紧程度，值越大螺旋越紧
				float spiralNoise = 6.0f;     // 控制星体在旋臂上的随机扰动

				// 随机选择一个旋臂
				int armIndex = rand() % numArms;
				float armOffset = DirectX::XM_2PI / numArms * armIndex; // 旋臂的起始角度偏移

				// 随机生成旋转角度 theta
				float theta = RandomFloat(0.0f, DirectX::XM_2PI); // 角度范围

				// 根据螺旋公式生成半径
				float r = spiralTightness * theta; // 螺旋的半径随角度增大

				// 为螺旋添加随机扰动，模拟星体的自然分布
				float noiseX = RandomFloat(-spiralNoise, spiralNoise);
				float noiseZ = RandomFloat(-spiralNoise, spiralNoise);

				// 计算星体位置，添加噪声
				float x = (r + noiseX) * cos(theta + armOffset);
				float z = (r + noiseZ) * sin(theta + armOffset);
				float y = RandomFloat(-b, b); // 控制银河系的厚度

				float distance = CalculateDistance(DirectX::XMFLOAT3(x, y, z), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

				y = (y / sqrt(sqrt(distance)));
				// 根据距离进行 y 轴缩放因子线性插值
				float yFactor = lerp(2.0f, 0.5f, r / a); // 距离越大，因子越接近1.0

				// 赋值给点的坐标
				point.pos = DirectX::XMFLOAT3(x, y * yFactor, z);
			}
			point.luminosity = RandomFloat(0.1f, 2.5f); // 调整亮度范围
		}
		

		// 生成随机数，用于决定星体的颜色类别
		float colorType = RandomFloat(0.0f, 1.0f);

		// 根据 colorType 确定恒星的颜色，模拟自然的恒星分布
		if (colorType < 0.1f) // 蓝白色恒星
		{
			point.color = DirectX::XMFLOAT4(RandomFloat(0.4f, 0.5f), RandomFloat(0.4f, 0.5f), RandomFloat(0.9f, 1.0f), 1.0f);
		}
		else if (colorType < 0.15f) // 白色恒星
		{
			point.color = DirectX::XMFLOAT4(RandomFloat(0.8f, 1.0f), RandomFloat(0.8f, 1.0f), RandomFloat(0.8f, 1.0f), 1.0f);
		}
		else if (colorType < 0.55f) // 黄色恒星（增大概率范围）
		{
			point.color = DirectX::XMFLOAT4(RandomFloat(0.95f, 1.0f), RandomFloat(0.7f, 0.9f), RandomFloat(0.4f, 0.6f), 1.0f);
		}
		else if (colorType < 0.85f) // 橙色恒星（增大概率范围，并使其更鲜艳）
		{
			point.color = DirectX::XMFLOAT4(RandomFloat(0.9f, 1.0f), RandomFloat(0.5f, 0.7f), RandomFloat(0.2f, 0.4f), 1.0f);
		}
		else // 红色恒星
		{
			point.color = DirectX::XMFLOAT4(RandomFloat(0.7f, 0.9f), RandomFloat(0.1f, 0.3f), RandomFloat(0.1f, 0.2f), 1.0f);
		}


		verticesPoints.push_back(point);
	}
	

	// 为每个点找到最近的 0-5 个点并生成线条
	for (int i = 0; i < verticesPoints.size(); ++i)
	{
		// 创建一个临时数组保存其他点的索引和距离
		std::vector<std::pair<int, float>> distances;

		// 逐个搜索找到最近的点并满足最小距离要求
		for (int j = 0; j < verticesPoints.size(); ++j)
		{
			if (i != j) // 不和自身比较
			{
				float distance = CalculateDistance(verticesPoints[i].pos, verticesPoints[j].pos);

				// 检查是否满足最小距离要求
				if (distance <= 1.0f)
				{
					distances.push_back({ j, distance });

					// 一旦找到了足够数量的点，停止搜索
					if (distances.size() >= 15)
					{
						break;
					}
				}
			}
		}

		// 按距离排序，确保从最近的点开始
		std::sort(distances.begin(), distances.end(),
			[](const std::pair<int, float>& a, const std::pair<int, float>& b) { return a.second < b.second; });

		// 根据最近的点数量生成线条
		int randomN = (RandomFloat(2, 3));
		int connections = randomN > distances.size() ? distances.size() : randomN;

		// 生成线条连接
		for (int k = 0; k < connections; ++k)
		{
			int nearestIndex = distances[k].first;
			verticesLines.push_back({ verticesPoints[i].pos, verticesPoints[i].color });
			verticesLines.push_back({ verticesPoints[nearestIndex].pos, verticesPoints[nearestIndex].color });
		}
	}
}

// 生成随机顶点和线条
void GenerateEVEMapVerticesAndLines(
	std::vector<UIWindowMap::PointVertexPosColor>& verticesPoints,
	std::vector<UIWindowMap::LineVertexPosColor>& verticesLines,
	std::vector<SolarSystemData>* solarSystem = nullptr,
	std::vector<SolarSystemJump>* solarSystemJumps = nullptr)
{
	// 清空数组
	verticesPoints.clear();
	verticesLines.clear();

	if (solarSystem == nullptr)
	{
		return;
	}

	// 创建一个映射，用于通过 SolarSystemID 查找 SolarSystemData
	std::unordered_map<int, DirectX::XMFLOAT3> solarSystemMap;
	for (int i = 0; i < solarSystem->size(); ++i)
	{
		UIWindowMap::PointVertexPosColor point;
		if (solarSystem != nullptr) {
			double factor = 40000000000000000.0;
			point.pos = DirectX::XMFLOAT3(
				static_cast<float>((*solarSystem)[i].x / factor),
				static_cast<float>((*solarSystem)[i].y / factor),
				static_cast<float>((*solarSystem)[i].z / factor));
			point.luminosity = (*solarSystem)[i].luminosity > 2.0f ? 2.0f : (*solarSystem)[i].luminosity; // 调整亮度范围
			point.luminosity = point.luminosity * 3.0f;

			// 将 SolarSystemID 与坐标映射起来，便于后续查找
			solarSystemMap[(*solarSystem)[i].solarSystemID] = point.pos;
		}

		// 生成随机数，用于决定星体的颜色类别
		float colorType = RandomFloat(0.0f, 1.0f);

		// 根据 colorType 确定恒星的颜色，模拟自然的恒星分布
		if (colorType < 0.1f) // 蓝白色恒星
		{
			point.color = DirectX::XMFLOAT4(RandomFloat(0.4f, 0.5f), RandomFloat(0.4f, 0.5f), RandomFloat(0.9f, 1.0f), 1.0f);
		}
		else if (colorType < 0.15f) // 白色恒星
		{
			point.color = DirectX::XMFLOAT4(RandomFloat(0.8f, 1.0f), RandomFloat(0.8f, 1.0f), RandomFloat(0.8f, 1.0f), 1.0f);
		}
		else if (colorType < 0.55f) // 黄色恒星（增大概率范围）
		{
			point.color = DirectX::XMFLOAT4(RandomFloat(0.95f, 1.0f), RandomFloat(0.7f, 0.9f), RandomFloat(0.4f, 0.6f), 1.0f);
		}
		else if (colorType < 0.85f) // 橙色恒星（增大概率范围，并使其更鲜艳）
		{
			point.color = DirectX::XMFLOAT4(RandomFloat(0.9f, 1.0f), RandomFloat(0.5f, 0.7f), RandomFloat(0.2f, 0.4f), 1.0f);
		}
		else // 红色恒星
		{
			point.color = DirectX::XMFLOAT4(RandomFloat(0.7f, 0.9f), RandomFloat(0.1f, 0.3f), RandomFloat(0.1f, 0.2f), 1.0f);
		}

		verticesPoints.push_back(point);
	}

	// 如果提供了星门跳跃数据，生成对应的线条
	if (solarSystemJumps != nullptr)
	{
		for (const auto& jump : *solarSystemJumps)
		{
			// 查找起点和终点的坐标
			auto fromIt = solarSystemMap.find(jump.fromSolarSystemID);
			auto toIt = solarSystemMap.find(jump.toSolarSystemID);

			// 如果起点和终点都找到了，生成线条
			if (fromIt != solarSystemMap.end() && toIt != solarSystemMap.end())
			{
				// 创建线条顶点并添加到线条列表中
				UIWindowMap::LineVertexPosColor fromVertex = { fromIt->second, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }; // 设置线条颜色
				UIWindowMap::LineVertexPosColor toVertex = { toIt->second, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) };

				verticesLines.push_back(fromVertex);
				verticesLines.push_back(toVertex);
			}
		}
	}
}


bool UIWindowMap::InitResource()
{
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

	// 绑定采样器状态到像素着色器
	m_pd3dImmediateContext->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());


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
	auto cam3st = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pCamera);

	// 映射常量缓冲区，确保成功后...
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->model = XMMatrixTranspose(XMMatrixIdentity());
	dataPtr->view = XMMatrixTranspose(cam3st->GetViewXM()); // 确保矩阵是列主序以适配HLSL默认
	dataPtr->projection = XMMatrixTranspose(cam3st->GetProjXM());

	m_pd3dImmediateContext->Unmap(matrixBuffer.Get(), 0);

	// 设置顶点着色器中的常量缓冲区
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());

	std::vector<PointVertexPosColor> verticesPoints;
	std::vector<LineVertexPosColor> verticesLines;
	GenerateEVEMapVerticesAndLines(verticesPoints, verticesLines, &m_solarSystem, &m_solarSystemJump);

	numPoint = verticesPoints.size();
	numLinePoint = verticesLines.size();

	/*std::vector<PointVertexPosColor> verticesPoints = {
	{ DirectX::XMFLOAT3(0.2f, 0.2f, -0.2f) ,DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), 3.0f },
	{ DirectX::XMFLOAT3(0.2f, 0.3f, 0.0f)  ,DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f),1.0f }, 
	};*/
	//{ DirectX::XMFLOAT3(2.0f, 2.0f, 0.0f) ,DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), 0.5f }
	//};
	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(PointVertexPosColor) * verticesPoints.size(); // 注意这里的变化
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitDataPoints;
	ZeroMemory(&InitDataPoints, sizeof(InitDataPoints));
	InitDataPoints.pSysMem = verticesPoints.data();
	auto mHR = (m_pd3dDevice->CreateBuffer(&vbd, &InitDataPoints, m_pVertexBufferPoints.GetAddressOf()));

	/*std::vector<LineVertexPosColor> verticesLines = {
	{ DirectX::XMFLOAT3(0.2f, 0.2f, -0.2f) ,DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), },
	{ DirectX::XMFLOAT3(0.2f, 0.3f, 0.0f)  ,DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) }
	};*/

	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(LineVertexPosColor) * verticesLines.size(); // 注意这里的变化
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;


	D3D11_SUBRESOURCE_DATA InitDataLines;
	ZeroMemory(&InitDataLines, sizeof(InitDataLines));
	InitDataLines.pSysMem = verticesLines.data();
	HR(m_pd3dDevice->CreateBuffer(&vbd, &InitDataLines, m_pVertexBufferLines.GetAddressOf()));



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
	if (SUCCEEDED(hr)) {
	}
	else {
		// 错误处理
	}

	//// ******************
	//// 给渲染管线各个阶段绑定好所需资源
	////

	//// 输入装配阶段的顶点缓冲区设置
	//UINT stride = sizeof(PointVertexPosColor);	// 跨越字节数
	//UINT offset = 0;						// 起始偏移量

	//m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	//// 设置图元类型，设定输入布局
	//m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	//// 将着色器绑定到渲染管线
	//m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	//m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

	ComPtr<ID3DBlob> blobPoints;
	ComPtr<ID3DBlob> blobLines;

	// 创建点的顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\StarMap\\StarMapPoint_VS.cso", L"HLSL\\StarMap\\StarMapPoint_VS.hlsl", "VS", "vs_5_0", blobPoints.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blobPoints->GetBufferPointer(), blobPoints->GetBufferSize(), nullptr, m_pVertexShaderPoints.GetAddressOf()));

	// 使用点的顶点着色器的 blob 创建点的顶点布局
	HR(m_pd3dDevice->CreateInputLayout(UIWindowMap::PointVertexPosColor::inputLayout, ARRAYSIZE(UIWindowMap::PointVertexPosColor::inputLayout),
		blobPoints->GetBufferPointer(), blobPoints->GetBufferSize(), m_pVertexLayoutPoints.GetAddressOf()));

	// 创建线的顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\StarMap\\StarMapLine_VS.cso", L"HLSL\\StarMap\\StarMapLine_VS.hlsl", "VS", "vs_5_0", blobLines.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blobLines->GetBufferPointer(), blobLines->GetBufferSize(), nullptr, m_pVertexShaderLines.GetAddressOf()));

	// 使用线的顶点着色器的 blob 创建线的顶点布局
	HR(m_pd3dDevice->CreateInputLayout(UIWindowMap::LineVertexPosColor::inputLayout, ARRAYSIZE(UIWindowMap::LineVertexPosColor::inputLayout),
		blobLines->GetBufferPointer(), blobLines->GetBufferSize(), m_pVertexLayoutLines.GetAddressOf()));

	// 创建点的像素着色器
	HR(CreateShaderFromFile(L"HLSL\\StarMap\\StarMapPoint_PS.cso", L"HLSL\\StarMap\\StarMapPoint_PS.hlsl", "PS", "ps_5_0", blobPoints.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blobPoints->GetBufferPointer(), blobPoints->GetBufferSize(), nullptr, m_pPixelShaderPoints.GetAddressOf()));

	// 创建线的像素着色器
	HR(CreateShaderFromFile(L"HLSL\\StarMap\\StarMapLine_PS.cso", L"HLSL\\StarMap\\StarMapLine_PS.hlsl", "PS", "ps_5_0", blobLines.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blobLines->GetBufferPointer(), blobLines->GetBufferSize(), nullptr, m_pPixelShaderLines.GetAddressOf()));

	// 创建几何着色器
	HR(CreateShaderFromFile(L"HLSL\\StarMap\\StarMapPoint_GS.cso", L"HLSL\\StarMap\\StarMapPoint_GS.hlsl", "GS", "gs_5_0", blobPoints.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateGeometryShader(blobPoints->GetBufferPointer(), blobPoints->GetBufferSize(), nullptr, m_pGeometryShader.GetAddressOf()));



	// ******************
	// 设置调试对象名
	//
	D3D11SetDebugObjectName(m_pVertexShaderPoints.Get(), "VertexShaderPoints");
	D3D11SetDebugObjectName(m_pVertexLayoutPoints.Get(), "VertexLayoutPoints");
	D3D11SetDebugObjectName(m_pVertexShaderLines.Get(), "VertexShaderLines");
	D3D11SetDebugObjectName(m_pVertexLayoutLines.Get(), "VertexLayoutLines");
	D3D11SetDebugObjectName(m_pPixelShaderPoints.Get(), "PixelShaderPoints");
	D3D11SetDebugObjectName(m_pPixelShaderLines.Get(), "PixelShaderLines");
	D3D11SetDebugObjectName(m_pGeometryShader.Get(), "GeometryShader");
	return true;
}

bool UIWindowMap::InitEffect()
{
	return true;
}

bool UIWindowMap::InitMap()
{
	m_solarSystem = getSolarSystems();
	m_solarSystemJump = getSolarSystemJumps();
	m_regions = getRegions();


	return true;
}
