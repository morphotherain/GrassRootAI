#include "UISkyBox.h"

using namespace DirectX;

std::vector<VertexPos> UISkyBox::GenerateButtonVertices() {
	// 完整定义立方体的顶点，共36个顶点（12个三角形）
	std::vector<VertexPos> vertices = {
		// 前面 (z = 1.0f)
		{ DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f) }, // 左上
		{ DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f) }, // 右上
		{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f) }, // 右下
		{ DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f) }, // 左上
		{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f) }, // 右下
		{ DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f) }, // 左下

		// 后面 (z = -1.0f)
		{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f) }, // 左上
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f) }, // 左下
		{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f) }, // 右下
		{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f) }, // 左上
		{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f) }, // 右下
		{ DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f) }, // 右上

		// 顶面 (y = 1.0f)
		{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f) }, // 左前
		{ DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f) }, // 右前
		{ DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f) }, // 右后
		{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f) }, // 左前
		{ DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f) }, // 右后
		{ DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f) }, // 左后

		// 底面 (y = -1.0f)
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f) }, // 左前
		{ DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f) }, // 左后
		{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f) }, // 右后
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f) }, // 左前
		{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f) }, // 右后
		{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f) }, // 右前

		// 左面 (x = -1.0f)
		{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f) }, // 上前
		{ DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f) }, // 上后
		{ DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f) }, // 下后
		{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f) }, // 上前
		{ DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f) }, // 下后
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f) }, // 下前

		// 右面 (x = 1.0f)
		{ DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f) }, // 上前
		{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f) }, // 下前
		{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f) }, // 下后
		{ DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f) }, // 上前
		{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f) }, // 下后
		{ DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f) }, // 上后
	};

	return vertices;
}

bool UISkyBox::Init()
{
	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	clickFlag = std::make_shared<bool>(false);

	return false;
}

void UISkyBox::OnResize()
{
}

void UISkyBox::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
}

void UISkyBox::DrawUI()
{
	auto m_pd3dDevice = D3DManager::getInstance().getDevice();
	auto m_pd3dImmediateContext = D3DManager::getInstance().getDeviceContext();

	// 1. 关闭深度测试以确保 UI 不受场景深度影响
	D3D11_DEPTH_STENCIL_DESC uiDepthStencilDesc;
	ZeroMemory(&uiDepthStencilDesc, sizeof(uiDepthStencilDesc));
	uiDepthStencilDesc.DepthEnable = FALSE; // 禁用深度测试
	uiDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 禁用深度写入
	uiDepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS; // 总是通过深度测试
	uiDepthStencilDesc.StencilEnable = FALSE;

	ComPtr<ID3D11DepthStencilState> pUIDepthStencilState;
	m_pd3dDevice->CreateDepthStencilState(&uiDepthStencilDesc, &pUIDepthStencilState);
	m_pd3dImmediateContext->OMSetDepthStencilState(pUIDepthStencilState.Get(), 0);

	// 更新常量缓冲区
	WorldViewProj* dataPtr;
	dataPtr = m_skyboxEffect->getConstantBuffer<WorldViewProj>()->Map();

	// 获取子类
	auto cam1st = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pCamera);

	// 获取视图矩阵和投影矩阵
	DirectX::XMMATRIX viewMatrix = cam1st->GetViewXM();
	DirectX::XMMATRIX projMatrix = cam1st->GetProjXM();
	// 从视图矩阵移除平移分量，以固定天空盒在相机中心
	viewMatrix.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	// 计算最终的 WorldViewProj 矩阵
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX worldViewProjMatrix = worldMatrix * viewMatrix * projMatrix;

	// 映射常量缓冲区，确保成功后...
	dataPtr->g_WorldViewProj = DirectX::XMMatrixTranspose(worldViewProjMatrix); // HLSL 使用列主序矩阵

	m_skyboxEffect->getConstantBuffer<WorldViewProj>()->Unmap();
	m_pd3dImmediateContext->PSSetShaderResources(0, 1, textureArraySRV.GetAddressOf());
	m_skyboxEffect->apply();
	m_pd3dImmediateContext->OMSetDepthStencilState(nullptr, 0);
}

void UISkyBox::cleanup()
{
}

bool UISkyBox::InitResource()
{
	auto m_pd3dDevice = D3DManager::getInstance().getDevice();

	// 加载立方体纹理
	ComPtr<ID3D11Resource> pTextureCube;
	HR(DirectX::CreateDDSTextureFromFile(m_pd3dDevice, L"Texture\\demoTex\\SpaceScene\\skybox.dds", &pTextureCube, &textureArraySRV));

	// 确保创建的 SRV 是 TEXTURECUBE 类型
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // 根据实际纹理格式调整
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1; // 使用所有 MIP 级别

	// 创建 SRV，确保是立方体纹理
	HR(m_pd3dDevice->CreateShaderResourceView(pTextureCube.Get(), &srvDesc, &textureArraySRV));

	std::vector<VertexPos> button_vertices = GenerateButtonVertices();

	m_skyboxEffect = std::make_shared<Effect>();

	m_skyboxEffect->addVertexShaderBuffer<VertexPos>(L"HLSL\\Sky\\Sky_VS.hlsl", L"HLSL\\Sky\\Sky_VS.cso");
	m_skyboxEffect->getVertexBuffer<VertexPos>()->setVertices(button_vertices);
	m_skyboxEffect->addConstantBuffer<WorldViewProj>();
	m_skyboxEffect->addPixelShader(L"HLSL\\Sky\\Sky_PS.hlsl", L"HLSL\\Sky\\Sky_PS.cso");
	m_skyboxEffect->Init();

	return true;
}

bool UISkyBox::InitEffect()
{
	return true;
}