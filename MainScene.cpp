#include "MainScene.h"

using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC MainScene::VertexPosColor::inputLayout[3] = {
	// λ���ֶ�
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// ���������ֶ�
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// ���������ֶ�
	{ "TEXINDEX", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

// ���峣���������Ľṹ
struct MatrixBufferType
{
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

struct MapVertexPosColor {
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
	float texIndex;    // ������������Ϊ�������洢
};

std::vector<MapVertexPosColor> GenerateVertices(int n) {
	std::vector<MapVertexPosColor> vertices;

	vertices.reserve(6); // ÿ���������������Σ�ÿ��������3������

	float x = -1.0f;
	float y = -108.0f/192.0f;
	float deltaY = 216.0f / 192.0f;

	// ��һ��������
	vertices.push_back({ XMFLOAT3(x, y, 0.0f),             XMFLOAT2(0.0f, 1.0f), 0 });
	vertices.push_back({ XMFLOAT3(x, (y + deltaY), 0.0f),       XMFLOAT2(0.0f, 0.0f), 0 });
	vertices.push_back({ XMFLOAT3((x + 2), y, 0.0f),       XMFLOAT2(1.0f, 1.0f), 0 });

	// �ڶ���������
	vertices.push_back({ XMFLOAT3((x + 2), y, 0.0f),       XMFLOAT2(1.0f, 1.0f), 0 });
	vertices.push_back({ XMFLOAT3(x, (y + deltaY), 0.0f),       XMFLOAT2(0.0f, 0.0f), 0 });
	vertices.push_back({ XMFLOAT3((x + 2), (y + deltaY), 0.0f), XMFLOAT2(1.0f, 0.0f), 0 });


	return vertices;
}


bool MainScene::Init()
{

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	return false;
}

void MainScene::OnResize()
{
}

void MainScene::UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard)
{
	// ��������¼�����ȡ���ƫ����
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);

	// ��ȡ����
	auto cam1st = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);
	auto camPos = cam1st->GetPosition();
	float factor = 1.0f / sqrt(abs(camPos.z) + 2.0f); //���������ҷ�ٶ�

	if (m_CameraMode == CameraMode::Free)
	{
		// ******************
		// ����������Ĳ���
		//

		// �����ƶ�
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
		// �����û���봰��ǰ��ΪABSOLUTEģʽ
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
	// ���������
	//

	XMFLOAT3 adjustedPos;
	XMStoreFloat3(&adjustedPos, XMVectorClamp(cam1st->GetPositionXM(),
		XMVectorSet(-0.0f, -0.0f, -1.0f, 0.0f), XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f)));
	cam1st->SetPosition(adjustedPos);



	// �˳���������Ӧ�򴰿ڷ���������Ϣ
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Escape))
		SendMessage(m_hMainWnd, WM_DESTROY, 0, 0);
}

void MainScene::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,0,255)
	static float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA = (255,255,255,255)
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), white);
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ���� camera �ǵ�ǰ�����е���Ӱ������
	DirectX::XMMATRIX viewMatrix = m_pCamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pCamera->GetProjXM();

	// ӳ�䳣��������
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_pd3dImmediateContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(hr))
	{
		MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
		dataPtr->view = XMMatrixTranspose(viewMatrix); // ת�þ�����ƥ��HLSL������
		dataPtr->projection = XMMatrixTranspose(projMatrix);

		// ȡ��ӳ�䳣��������
		m_pd3dImmediateContext->Unmap(matrixBuffer.Get(), 0);

		// �������������󶨵�������ɫ��
		m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());
	}


	// ����nxn���̸�
	int n = 4;
	int verticesPerSquare = 2 * 3; // ÿ�����������������Σ�ÿ��������3������
	int totalVertices = n * n * verticesPerSquare;
	// ����������
	m_pd3dImmediateContext->Draw(6, 0);
	HR(m_pSwapChain->Present(1, 0));
}

void MainScene::cleanup()
{
}



bool MainScene::InitResource()
{
	auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);
	m_pCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	camera->SetPosition(XMFLOAT3(100.0f, 100.0f, 10.0f));
	camera->SetFrustum(XM_PI / 3, AspectRatio(), 1.0f, 1000.0f);
	camera->LookTo(XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, +0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	camera->SetPosition(XMFLOAT3(1.0f, 1.0f, -1.0f));


	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;  // ʹ�õ����
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;   // ����U�����ϵ�ѭ��
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;   // ����V�����ϵ�ѭ��
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;   // ����W�����ϵ�ѭ������3D������Ч
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ComPtr<ID3D11SamplerState> pSamplerState;
	HRESULT hr = m_pd3dDevice->CreateSamplerState(&sampDesc, pSamplerState.GetAddressOf());
	if (FAILED(hr))
	{
		// �������
	}

	// �󶨲�����״̬��������ɫ��
	m_pd3dImmediateContext->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());

	m_ddsLoader.Init(*m_pd3dDevice.GetAddressOf(), *m_pd3dImmediateContext.GetAddressOf());

	std::vector<std::string> textureFileNames = {
		"demoTex\\MainScene\\background.dds"
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
	// ʹ���豸����������
	m_pd3dDevice->CreateBuffer(&matrixBufferDesc, nullptr, matrixBuffer.GetAddressOf());

	// ���³���������
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	hr = m_pd3dImmediateContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	// ȷ�����hr��ֵ...

	// ��ȡ����
	auto cam1st = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);

	// ӳ�䳣����������ȷ���ɹ���...
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->view = XMMatrixTranspose(cam1st->GetViewXM()); // ȷ��������������������HLSLĬ��
	dataPtr->projection = XMMatrixTranspose(cam1st->GetProjXM());


	m_pd3dImmediateContext->Unmap(matrixBuffer.Get(), 0);

	// ���ö�����ɫ���еĳ���������
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());




	std::vector<MapVertexPosColor> vertices = GenerateVertices(4);
	// ���ö��㻺��������
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexPosColor) * vertices.size(); // ע������ı仯
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// �½����㻺����
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices.data();
	HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));


	// ******************
	// ����Ⱦ���߸����׶ΰ󶨺�������Դ
	//

	// ����װ��׶εĶ��㻺��������
	UINT stride = sizeof(VertexPosColor);	// ��Խ�ֽ���
	UINT offset = 0;						// ��ʼƫ����

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	// ����ͼԪ���ͣ��趨���벼��
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	// ����ɫ���󶨵���Ⱦ����
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);


	m_pd3dImmediateContext->PSSetShaderResources(0, 1, textureArraySRV.GetAddressOf());
	// ******************
	// ���õ��Զ�����
	//
	D3D11SetDebugObjectName(m_pVertexLayout.Get(), "VertexPosColorLayout");
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	D3D11SetDebugObjectName(m_pVertexShader.Get(), "Trangle_VS");
	D3D11SetDebugObjectName(m_pPixelShader.Get(), "Trangle_PS");
	return true;
}

bool MainScene::InitEffect()
{
	ComPtr<ID3DBlob> blob;

	// ����������ɫ��
	HR(CreateShaderFromFile(L"HLSL\\Triangle_VS.cso", L"HLSL\\Triangle_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	// �������󶨶��㲼��
	HR(m_pd3dDevice->CreateInputLayout(VertexPosColor::inputLayout, ARRAYSIZE(VertexPosColor::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));

	// ����������ɫ��
	HR(CreateShaderFromFile(L"HLSL\\Triangle_PS.cso", L"HLSL\\Triangle_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

	return true;
}
