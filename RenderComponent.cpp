#include "RenderComponent.h"
using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC RenderComponent::VertexPosColor::inputLayout[3] = {
	// 位置字段
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 纹理坐标字段
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 纹理索引字段
	{ "TEXINDEX", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


struct MapVertexPosColor {
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
	float texIndex;    // 纹理索引，作为浮点数存储
};


std::vector<MapVertexPosColor> GenerateRegion() {
	float f_n = 2.0f;
	std::vector<MapVertexPosColor> vertices;

	vertices.reserve(6); // 每个格子两个三角形，每个三角形3个顶点


	// 左下角的点坐标
	float x = -0.5f;
	float y = -0.5f; // 使得(0,0)在左上角

	float indexTex = 0;

	// 添加两个三角形来填充这个格子
	// 第一个三角形
	vertices.push_back({ XMFLOAT3(x, y, 0.0f),             XMFLOAT2(0.0f, 1.0f), indexTex });
	vertices.push_back({ XMFLOAT3(x, y + 1.0f, 0.0f),       XMFLOAT2(0.0f, 0.0f), indexTex });
	vertices.push_back({ XMFLOAT3(x + 1.0f, y, 0.0f),       XMFLOAT2(1.0f, 1.0f), indexTex });

	// 第二个三角形
	vertices.push_back({ XMFLOAT3(x + 1.0f, y , 0.0f),       XMFLOAT2(1.0f, 1.0f), indexTex });
	vertices.push_back({ XMFLOAT3(x, y + 1.0f, 0.0f),       XMFLOAT2(0.0f, 0.0f), indexTex });
	vertices.push_back({ XMFLOAT3(x + 1.0f, y + 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), indexTex });


	return vertices;
}



RenderComponent::RenderComponent(HINSTANCE _hInstance) :
    hInstance(_hInstance),
    m_CameraMode(CameraMode::Free)
{
}

void RenderComponent::setd3dResource(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, IDXGISwapChain* pSwapChain, HWND hMainWnd, ID3D11RenderTargetView* pRenderTargetView, ID3D11DepthStencilView* pDepthStencilView)
{
    m_pd3dDevice = pd3dDevice;
    m_pd3dImmediateContext = pd3dImmediateContext;
    m_pSwapChain = pSwapChain;
    m_hMainWnd = hMainWnd;
    m_pRenderTargetView = pRenderTargetView;
    m_pDepthStencilView = pDepthStencilView;
}

void RenderComponent::setcameraResource(int ClientWidth, int ClientHeight, std::shared_ptr<Camera> pCamera)
{
    m_ClientWidth = ClientWidth;
    m_ClientHeight = ClientHeight;
    m_pCamera = pCamera;
}


bool RenderComponent::Init()
{

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	return false;
}

void RenderComponent::OnResize()
{
}

void RenderComponent::Update(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard)
{
	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);

	if (keyState.IsKeyDown(Keyboard::A))
	{
		left = true;
	}
	if (keyState.IsKeyDown(Keyboard::D))
	{
		left = false;
	}
	

	tick++;
}

void RenderComponent::Draw()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	// 假设 camera 是当前场景中的摄影机对象
	DirectX::XMMATRIX viewMatrix = m_pCamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pCamera->GetProjXM();

	// 映射常量缓冲区

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_pd3dImmediateContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(hr))
	{
		MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
		dataPtr->model = XMMatrixTranspose(modelMartix);
		dataPtr->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
		dataPtr->projection = XMMatrixTranspose(projMatrix);
		dataPtr->TexIndex = left ? (((tick / 60) % 2)) : (((tick / 60) % 2) + 2);


		// 取消映射常量缓冲区
		m_pd3dImmediateContext->Unmap(matrixBuffer.Get(), 0);

		// 将常量缓冲区绑定到顶点着色器
		m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());
	}


	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(VertexPosColor);	// 跨越字节数
	UINT offset = 0;						// 起始偏移量

	//按钮绘制

	m_pd3dImmediateContext->PSSetShaderResources(0, 1, textureArraySRV.GetAddressOf()); //绑定纹理

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);

	m_pd3dImmediateContext->Draw(6, 0);

}

void RenderComponent::cleanup()
{
}



bool RenderComponent::InitResource()
{

	m_ddsLoader.Init(*m_pd3dDevice.GetAddressOf(), *m_pd3dImmediateContext.GetAddressOf());

	std::vector<std::string> textureButtonFileNames = {
		"demoTex\\player\\playerLeft1.dds",
		"demoTex\\player\\playerLeft2.dds",
		"demoTex\\player\\playerRight1.dds",
		"demoTex\\player\\playerRight2.dds",
	};

	m_ddsLoader.InitTex32ArrayFromFiles(textureButtonFileNames, textureArraySRV);


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
	dataPtr->model = XMMatrixTranspose(XMMatrixIdentity());
	dataPtr->view = XMMatrixTranspose(cam1st->GetViewXM()); // 确保矩阵是列主序以适配HLSL默认
	dataPtr->projection = XMMatrixTranspose(cam1st->GetProjXM());
	dataPtr->TexIndex = 0;


	m_pd3dImmediateContext->Unmap(matrixBuffer.Get(), 0);

	// 设置顶点着色器中的常量缓冲区
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());


	std::vector<MapVertexPosColor> button_vertices = GenerateRegion();
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
	HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));


	return true;
}

bool RenderComponent::InitEffect()
{


	return true;
}