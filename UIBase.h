#pragma once
#include "Scene.h"
class UIBase
{
public:
	UIBase() = default;
	UIBase(HINSTANCE hInstance);
	~UIBase() = default;

	virtual bool Init() = 0;
	virtual void OnResize() = 0;
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene) = 0;
	virtual void DrawUI() = 0;
	virtual void cleanup() = 0;

	void setd3dResource(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, IDXGISwapChain* pSwapChain, HWND hMainWnd, ID3D11RenderTargetView* pRenderTargetView, ID3D11DepthStencilView* m_pDepthStencilView);
	void setcameraResource(int m_ClientWidth, int m_ClientHeight, std::shared_ptr<Camera> pCamera);

	enum class CameraMode { FirstPerson, ThirdPerson, Free };
	float AspectRatio()const { return static_cast<float>(m_ClientWidth) / m_ClientHeight; }

protected:

	HWND      m_hMainWnd;        // �����ھ��
	HINSTANCE hInstance;

	ddsLoader m_ddsLoader;                                //���ʼ���

	CameraMode m_CameraMode;									// �����ģʽ
	std::shared_ptr<Camera> m_pCamera;						    // �����

	ComPtr<ID3D11Device> m_pd3dDevice;
	ComPtr<ID3D11DeviceContext> m_pd3dImmediateContext;   // D3D11�豸������
	ComPtr<IDXGISwapChain> m_pSwapChain;                  // D3D11������

	// ������Դ
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;        // ���ģ�建����
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;   // ��ȾĿ����ͼ
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;   // ���ģ����ͼ
	D3D11_VIEWPORT m_ScreenViewport;                      // �ӿ�

	int m_ClientWidth;                                    // �ӿڿ��
	int m_ClientHeight;                                   // �ӿڸ߶�

	ComPtr<ID3D11InputLayout> m_pVertexLayout;	// �������벼��
	ComPtr<ID3D11Buffer> m_pVertexBuffer;		// ���㻺����
	ComPtr<ID3D11VertexShader> m_pVertexShader;	// ������ɫ��
	ComPtr<ID3D11PixelShader> m_pPixelShader;	// ������ɫ��


	// ��������������
	ComPtr<ID3D11Buffer> matrixBuffer;

	DirectX::Mouse::ButtonStateTracker m_MouseTracker;			// ���״̬׷����
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;	// ����״̬׷����


};

