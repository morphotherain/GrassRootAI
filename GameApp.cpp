#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;


const D3D11_INPUT_ELEMENT_DESC GameApp::VertexPosColor::inputLayout[3] = {
	// 位置字段
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 纹理坐标字段
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 纹理索引字段
	{ "TEXINDEX", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;


	return true;
}

void GameApp::OnResize()
{
	D3DApp::OnResize();
}

void GameApp::UpdateScene(float dt)
{

}

void GameApp::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,0,255)
	static float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA = (255,255,255,255)
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), white);
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);



	// 对于nxn棋盘格
	int n = 4;
	int verticesPerSquare = 2 * 3; // 每个格子有两个三角形，每个三角形3个顶点
	int totalVertices = n * n * verticesPerSquare;
	// 绘制三角形
	m_pd3dImmediateContext->Draw(totalVertices, 0);
	HR(m_pSwapChain->Present(1, 0));
}

bool GameApp::InitEffect()
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

struct MapVertexPosColor {
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
	float texIndex;    // 纹理索引，作为浮点数存储
};

std::vector<MapVertexPosColor> GenerateChessboardVertices(int n) {
	float f_n = n/2.0f;
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
			float x = j - n/2;
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

bool GameApp::InitResource()
{
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

	m_ddsLoader.Init(*m_pd3dDevice.GetAddressOf(),*m_pd3dImmediateContext.GetAddressOf());

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


	std::vector<MapVertexPosColor> vertices = GenerateChessboardVertices(4);
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
