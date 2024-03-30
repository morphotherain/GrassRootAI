//***************************************************************************************
// Camera.h by X_Jun(MKXJun) (C) 2018-2020 All Rights Reserved.
// Licensed under the MIT License.
//
// �ṩ��һ�˳�(�����ӽ�)�͵����˳������
// Provide 1st person(free view) and 3rd person cameras.
//***************************************************************************************

#ifndef CAMERA_H
#define CAMERA_H

#include <d3d11_1.h>
#include <DirectXMath.h>
#include "Transform.h"


class Camera
{
public:
	Camera() = default;
	virtual ~Camera() = 0;

	//
	// ��ȡ�����λ��
	//

	DirectX::XMVECTOR GetPositionXM() const;
	DirectX::XMFLOAT3 GetPosition() const;

	//
	// ��ȡ�������ת
	//

	// ��ȡ��X����ת��ŷ���ǻ���
	float GetRotationX() const;
	// ��ȡ��Y����ת��ŷ���ǻ���
	float GetRotationY() const;

	//
	// ��ȡ�����������������
	//

	DirectX::XMVECTOR GetRightAxisXM() const;
	DirectX::XMFLOAT3 GetRightAxis() const;
	DirectX::XMVECTOR GetUpAxisXM() const;
	DirectX::XMFLOAT3 GetUpAxis() const;
	DirectX::XMVECTOR GetLookAxisXM() const;
	DirectX::XMFLOAT3 GetLookAxis() const;

	//
	// ��ȡ����
	//

	DirectX::XMMATRIX GetViewXM() const;
	DirectX::XMMATRIX GetProjXM() const;
	DirectX::XMMATRIX GetViewProjXM() const;

	// ��ȡ�ӿ�
	D3D11_VIEWPORT GetViewPort() const;


	// ������׶��
	void SetFrustum(float fovY, float aspect, float nearZ, float farZ);

	// �����ӿ�
	void SetViewPort(const D3D11_VIEWPORT& viewPort);
	void SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

protected:

	// ������ı任
	Transform m_Transform = {};
	
	// ��׶������
	float m_NearZ = 0.0f;
	float m_FarZ = 0.0f;
	float m_Aspect = 0.0f;
	float m_FovY = 0.0f;

	// ��ǰ�ӿ�
	D3D11_VIEWPORT m_ViewPort = {};

};

class FirstPersonCamera : public Camera
{
public:
	FirstPersonCamera() = default;
	~FirstPersonCamera() override;

	// ���������λ��
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& pos);
	// ����������ĳ���
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target,const DirectX::XMFLOAT3& up);
	void LookTo(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& to, const DirectX::XMFLOAT3& up);
	// ƽ��
	void Strafe(float d);
	// ֱ��(ƽ���ƶ�)
	void Walk(float d);
	// ǰ��(��ǰ���ƶ�)
	void MoveForward(float d);
	// ���¹۲�
	// ��radֵ���Ϲ۲�
	// ��radֵ���¹۲�
	void Pitch(float rad);
	// ���ҹ۲�
	// ��radֵ���ҹ۲�
	// ��radֵ����۲�
	void RotateY(float rad);

	void MoveX(float d) { m_Transform.Translate(DirectX::XMFLOAT3{ 1.0f, 0.0f, 0.0f }, d); };
	void MoveY(float d) { m_Transform.Translate(DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f }, d); };
	void MoveZ(float d) { m_Transform.Translate(DirectX::XMFLOAT3{ 0.0f, 0.0f, 1.0f }, d); };
};

class ThirdPersonCamera : public Camera
{
public:
	ThirdPersonCamera() = default;
	~ThirdPersonCamera() override;

	// ��ȡ��ǰ���������λ��
	DirectX::XMFLOAT3 GetTargetPosition() const;
	// ��ȡ������ľ���
	float GetDistance() const;
	// �����崹ֱ��ת(ע����x����תŷ���ǻ���������[0, pi/3])
	void RotateX(float rad);
	// ������ˮƽ��ת
	void RotateY(float rad);
	// ��������
	void Approach(float dist);
	// ���ó�ʼ��X��Ļ���(ע����x����תŷ���ǻ���������[0, pi/3])
	void SetRotationX(float rad);
	// ���ó�ʼ��Y��Ļ���
	void SetRotationY(float rad);
	// ���ò��󶨴����������λ��
	void SetTarget(const DirectX::XMFLOAT3& target);
	// ���ó�ʼ����
	void SetDistance(float dist);
	// ������С����������
	void SetDistanceMinMax(float minDist, float maxDist);

private:
	DirectX::XMFLOAT3 m_Target = {};
	float m_Distance = 0.0f;
	// ��С������룬����������
	float m_MinDist = 0.0f, m_MaxDist = 0.0f;
};


#endif
