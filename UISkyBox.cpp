#include "UISkyBox.h"

using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC UISkyBox::VertexPos::inputLayout[1] = {
	// 位置字段
	 { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};




std::vector<UISkyBox::VertexPos> UISkyBox::GenerateButtonVertices() {
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

void UISkyBox::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene)
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

void UISkyBox::DrawUI()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

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
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	HR(m_pd3dImmediateContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	// 确保检查hr的值...

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
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->g_WorldViewProj = DirectX::XMMatrixTranspose(worldViewProjMatrix); // HLSL 使用列主序矩阵

	m_pd3dImmediateContext->Unmap(matrixBuffer.Get(), 0);

	// 2. 设置输入装配阶段
	UINT stride = sizeof(VertexPos); // 例如：UI 顶点可能包含位置和颜色
	UINT offset = 0;

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 例如：三角形列表

	// 3. 设置输入布局
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());

	// 4. 绑定 UI 着色器
	m_pd3dImmediateContext->VSSetShader(m_pVertexShaderSky.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(m_pPixelShaderSky.Get(), nullptr, 0);

	// 5. 设置 UI 常量缓冲区（如果需要）
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());

	// 6. 绑定 UI 纹理和采样器（如果有）
	m_pd3dImmediateContext->PSSetShaderResources(0, 1, textureArraySRV.GetAddressOf());

	// 7. 绘制 UI 元素
	m_pd3dImmediateContext->Draw(36, 0);

	// 8. 恢复深度模板状态（如果需要继续绘制其他场景对象）
	// 这里可以重设回默认的深度状态，以便后续其他对象的正确渲染。
	m_pd3dImmediateContext->OMSetDepthStencilState(nullptr, 0);

}

void UISkyBox::cleanup()
{
}



bool UISkyBox::InitResource()
{
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
	HR(m_pd3dImmediateContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	// 确保检查hr的值...

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
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->g_WorldViewProj = DirectX::XMMatrixTranspose(worldViewProjMatrix); // HLSL 使用列主序矩阵

	m_pd3dImmediateContext->Unmap(matrixBuffer.Get(), 0);




	m_ddsLoader.Init(*m_pd3dDevice.GetAddressOf(), *m_pd3dImmediateContext.GetAddressOf());

	std::vector<std::string> textureButtonFileNames = {
		TexPath
	};

	// 加载立方体纹理
	ComPtr<ID3D11Resource> pTextureCube;
	HR( DirectX::CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\demoTex\\SpaceScene\\skybox.dds", &pTextureCube, &textureArraySRV));


	// 确保创建的 SRV 是 TEXTURECUBE 类型
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // 根据实际纹理格式调整
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1; // 使用所有 MIP 级别

	// 创建 SRV，确保是立方体纹理
	HR( m_pd3dDevice->CreateShaderResourceView(pTextureCube.Get(), &srvDesc, &textureArraySRV));

	std::vector<VertexPos> button_vertices = GenerateButtonVertices();
	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexPos) * button_vertices.size(); // 注意这里的变化
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = button_vertices.data();
	HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));


	return true;
}

bool UISkyBox::InitEffect()
{
	ComPtr<ID3DBlob> blob;
	// 创建顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\Sky\\Sky_VS.cso", L"HLSL\\Sky\\Sky_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShaderSky.GetAddressOf()));

	// 创建并绑定顶点布局
	HR(m_pd3dDevice->CreateInputLayout(VertexPos::inputLayout, ARRAYSIZE(VertexPos::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));

	// 创建像素着色器
	HR(CreateShaderFromFile(L"HLSL\\Sky\\Sky_PS.cso", L"HLSL\\Sky\\Sky_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShaderSky.GetAddressOf()));

	return true;
}