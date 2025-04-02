#include "Camera.h"
#include "Camera.h"
using namespace DirectX;

Camera::~Camera()
{
}

XMVECTOR Camera::GetPositionXM() const
{
	return m_Transform.GetPositionXM();
}

XMFLOAT3 Camera::GetPosition() const
{
	return m_Transform.GetPosition();
}

float Camera::GetRotationX() const
{
	return m_Transform.GetRotation().x;
}

float Camera::GetRotationY() const
{
	return m_Transform.GetRotation().y;
}


XMVECTOR Camera::GetRightAxisXM() const
{
	return m_Transform.GetRightAxisXM();
}

XMFLOAT3 Camera::GetRightAxis() const
{
	return m_Transform.GetRightAxis();
}

XMVECTOR Camera::GetUpAxisXM() const
{
	return m_Transform.GetUpAxisXM();
}

XMFLOAT3 Camera::GetUpAxis() const
{
	return m_Transform.GetUpAxis();
}

XMVECTOR Camera::GetLookAxisXM() const
{
	return m_Transform.GetForwardAxisXM();
}

XMFLOAT3 Camera::GetLookAxis() const
{
	return m_Transform.GetForwardAxis();
}

XMMATRIX Camera::GetViewXM() const
{
	return m_Transform.GetWorldToLocalMatrixXM();
}

XMMATRIX Camera::GetProjXM() const
{
	return XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
}

XMMATRIX Camera::GetViewProjXM() const
{
	return GetViewXM() * GetProjXM();
}

D3D11_VIEWPORT Camera::GetViewPort() const
{
	return m_ViewPort;
}

void Camera::SetFrustum(float fovY, float aspect, float nearZ, float farZ)
{
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = nearZ;
	m_FarZ = farZ;
}

void Camera::SetViewPort(const D3D11_VIEWPORT& viewPort)
{
	m_ViewPort = viewPort;
}

void Camera::SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
	m_ViewPort.TopLeftX = topLeftX;
	m_ViewPort.TopLeftY = topLeftY;
	m_ViewPort.Width = width;
	m_ViewPort.Height = height;
	m_ViewPort.MinDepth = minDepth;
	m_ViewPort.MaxDepth = maxDepth;
}


// ******************
// 第一人称/自由视角摄像机
//

FirstPersonCamera::~FirstPersonCamera()
{
}

void FirstPersonCamera::SetPosition(float x, float y, float z)
{
	SetPosition(XMFLOAT3(x, y, z));
}

void FirstPersonCamera::SetPosition(const XMFLOAT3& pos)
{
	m_Transform.SetPosition(pos);
}

void FirstPersonCamera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	m_Transform.SetPosition(pos);
	m_Transform.LookAt(target, up);
}

void FirstPersonCamera::LookTo(const XMFLOAT3& pos, const XMFLOAT3& to, const XMFLOAT3& up)
{
	m_Transform.SetPosition(pos);
	m_Transform.LookTo(to, up);
}

void FirstPersonCamera::Strafe(float d)
{
	m_Transform.Translate(m_Transform.GetRightAxis(), d);
}

void FirstPersonCamera::Walk(float d)
{
	XMVECTOR rightVec = m_Transform.GetRightAxisXM();
	XMVECTOR frontVec = XMVector3Normalize(XMVector3Cross(rightVec, g_XMIdentityR1));
	XMFLOAT3 front;
	XMStoreFloat3(&front, frontVec);
	m_Transform.Translate(front, d);
}

void FirstPersonCamera::MoveForward(float d)
{
	m_Transform.Translate(m_Transform.GetForwardAxis(), d);
}




void FirstPersonCamera::Pitch(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotation();
	// 将绕x轴旋转弧度限制在[-7pi/18, 7pi/18]之间
	rotation.x += rad;
	if (rotation.x > XM_PI * 7 / 18)
		rotation.x = XM_PI * 7 / 18;
	else if (rotation.x < -XM_PI * 7 / 18)
		rotation.x = -XM_PI * 7 / 18;

	m_Transform.SetRotation(rotation);
}

void FirstPersonCamera::RotateY(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotation();
	rotation.y = XMScalarModAngle(rotation.y + rad);
	m_Transform.SetRotation(rotation);
}





// ******************
// 第三人称摄像机
//

ThirdPersonCamera::~ThirdPersonCamera()
{
}

XMFLOAT3 ThirdPersonCamera::GetTargetPosition() const
{
	return m_Target;
}

float ThirdPersonCamera::GetDistance() const
{
	return m_Distance;
}

void ThirdPersonCamera::RotateX(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotation();
	// 将绕x轴旋转弧度限制在[0, pi/3]之间
	rotation.x += rad;
	if (rotation.x < 0.0f)
		rotation.x = 0.0f;
	else if (rotation.x > XM_PI / 3)
		rotation.x = XM_PI / 3;

	m_Transform.SetRotation(rotation);
	m_Transform.SetPosition(m_Target);
	m_Transform.Translate(m_Transform.GetForwardAxis(), -m_Distance);
}

void ThirdPersonCamera::RotateY(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotation();
	rotation.y = XMScalarModAngle(rotation.y + rad);

	m_Transform.SetRotation(rotation);
	m_Transform.SetPosition(m_Target);
	m_Transform.Translate(m_Transform.GetForwardAxis(), -m_Distance);
}

void ThirdPersonCamera::Approach(float dist)
{
	m_Distance += dist;
	// 限制距离在[m_MinDist, m_MaxDist]之间
	if (m_Distance < m_MinDist)
		m_Distance = m_MinDist;
	else if (m_Distance > m_MaxDist)
		m_Distance = m_MaxDist;

	m_Transform.SetPosition(m_Target);
	m_Transform.Translate(m_Transform.GetForwardAxis(), -m_Distance);
}

void ThirdPersonCamera::SetRotationX(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotation();
	// 将绕x轴旋转弧度限制在[0, pi/3]之间
	rotation.x = rad;
	if (rotation.x < 0.0f)
		rotation.x = 0.0f;
	else if (rotation.x > XM_PI / 3)
		rotation.x = XM_PI / 3;

	m_Transform.SetRotation(rotation);
	m_Transform.SetPosition(m_Target);
	m_Transform.Translate(m_Transform.GetForwardAxis(), -m_Distance);
}

void ThirdPersonCamera::SetRotationY(float rad)
{
	XMFLOAT3 rotation = m_Transform.GetRotation();
	rotation.y = XMScalarModAngle(rad);
	m_Transform.SetRotation(rotation);
	m_Transform.SetPosition(m_Target);
	m_Transform.Translate(m_Transform.GetForwardAxis(), -m_Distance);
}

void ThirdPersonCamera::SetTarget(const XMFLOAT3& target)
{
	m_Target = target;
}

void ThirdPersonCamera::SetDistance(float dist)
{
	m_Distance = dist;
}

void ThirdPersonCamera::SetDistanceMinMax(float minDist, float maxDist)
{
	m_MinDist = minDist;
	m_MaxDist = maxDist;
}

// 设置正交投影参数
void OrthographicCamera::SetOrthographic(float left, float right, float bottom, float top, float nearZ, float farZ)
{
	m_Left = left;
	m_Right = right;
	m_Bottom = bottom;
	m_Top = top;
	m_NearZ = nearZ;
	m_FarZ = farZ;
}

// 重写获取投影矩阵的方法
DirectX::XMMATRIX OrthographicCamera::GetProjXM() const
{
	return DirectX::XMMatrixOrthographicOffCenterLH(m_Left, m_Right, m_Bottom, m_Top, m_NearZ, m_FarZ);
}

// 重写获取视图矩阵的方法
DirectX::XMMATRIX OrthographicCamera::GetViewXM() const
{
	// 对于 UI 摄像机，通常视图矩阵为单位矩阵
	return DirectX::XMMatrixIdentity();
}

// 将三维坐标转换为标准化设备坐标(NDC)范围的函数
DirectX::XMFLOAT2 Convert3DToNDC(const DirectX::XMFLOAT3& worldPos, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projMatrix)
{
	// 将三维世界坐标转换为裁剪空间
	DirectX::XMVECTOR worldPosition = XMLoadFloat3(&worldPos);
	DirectX::XMVECTOR clipPosition = XMVector3Transform(worldPosition, viewMatrix * projMatrix);

	// 获取裁剪空间坐标的各个分量（这里假设采用透视投影）
	DirectX::XMFLOAT4  clipPos;
	XMStoreFloat4(&clipPos, clipPosition);

	if (clipPos.w < 0.0f)
	{
		return DirectX::XMFLOAT2(FLT_MAX, FLT_MAX);
	}

	// 手动透视除法
	float x_ndc = clipPos.x / clipPos.w;
	float y_ndc = clipPos.y / clipPos.w;

	// 返回的ndcPos.x 和 ndcPos.y 直接是 NDC 范围 [-1, 1]
	return DirectX::XMFLOAT2(x_ndc, y_ndc);
}

// 将NDC坐标转换为屏幕实际坐标的函数
DirectX::XMFLOAT2 ConvertNDCToScreen(const DirectX::XMFLOAT2& ndcPos, int screenWidth, int screenHeight)
{
	// NDC坐标范围是 [-1, 1]，转换到 [0, 1] 范围
	float x_screenNormalized = (ndcPos.x + 1.0f) / 2.0f;
	float y_screenNormalized = (1.0f - (ndcPos.y + 1.0f) / 2.0f);

	// 转换到屏幕像素坐标
	float x_screen = x_screenNormalized * screenWidth;
	float y_screen = y_screenNormalized * screenHeight;

	return DirectX::XMFLOAT2(x_screen, y_screen);
}
