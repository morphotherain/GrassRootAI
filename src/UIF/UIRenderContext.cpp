#include "UIRenderContext.h"
#include "Camera.h"
#include "Effect.h"
#include "VertexData.h"
#include "ConstantBufferData.h"
#include <DirectXMath.h>
#include <sstream>

using namespace DirectX;

static std::vector<PosTexIndex> MakeSpriteVertices(float w, float h, float u0, float v0, float u1, float v1)
{
	return GenerateUIVerticesUV(0.0f, 0.0f, w, h, u0, v0, u1, v1);
}

std::string UIRenderContext::MakeSpriteMeshKey(const std::string& texturePath, float w, float h,
	float u0, float v0, float u1, float v1)
{
	std::ostringstream ss;
	ss << texturePath << '|' << w << 'x' << h << '|' << u0 << ',' << v0 << ',' << u1 << ',' << v1;
	return ss.str();
}

void UIRenderContext::Initialize(int clientWidth, int clientHeight)
{
	m_clientWidth = clientWidth;
	m_clientHeight = clientHeight;
	m_uiCamera = std::make_shared<OrthographicCamera>();
	OnResize(clientWidth, clientHeight);
}

void UIRenderContext::OnResize(int clientWidth, int clientHeight)
{
	m_clientWidth = clientWidth;
	m_clientHeight = clientHeight;
	if (!m_uiCamera)
	{
		return;
	}
	m_uiCamera->SetOrthographic(
		0.0f,
		static_cast<float>(clientWidth),
		static_cast<float>(clientHeight),
		0.0f,
		0.0f,
		1.0f);
}

std::shared_ptr<Effect> UIRenderContext::GetOrCreateSpriteEffect(const std::string& texturePath, float w, float h,
	float u0, float v0, float u1, float v1)
{
	const std::string meshKey = MakeSpriteMeshKey(texturePath, w, h, u0, v0, u1, v1);
	auto it = m_spriteEffects.find(meshKey);
	if (it != m_spriteEffects.end())
	{
		return it->second;
	}

	auto effect = std::make_shared<Effect>();
	effect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	effect->getVertexBuffer<PosTexIndex>()->setVertices(MakeSpriteVertices(w, h, u0, v0, u1, v1));
	effect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	effect->addConstantBuffer<ConstantMVPIndex>();
	effect->addTextures({ texturePath });
	effect->addBlendState();
	effect->addSamplerState();
	effect->Init();

	m_spriteEffects[meshKey] = effect;
	m_spriteMeshes[meshKey] = { w, h, u0, v0, u1, v1 };
	return effect;
}

void UIRenderContext::DrawUIQuad(const std::string& texturePath, float x, float y, float w, float h,
	float scaleX, float scaleY, float rotationDeg, float u0, float v0, float u1, float v1)
{
	if (texturePath.empty() || w <= 0.0f || h <= 0.0f)
	{
		return;
	}

	const std::string meshKey = MakeSpriteMeshKey(texturePath, w, h, u0, v0, u1, v1);
	auto effect = GetOrCreateSpriteEffect(texturePath, w, h, u0, v0, u1, v1);
	if (!effect)
	{
		return;
	}

	const auto meshIt = m_spriteMeshes.find(meshKey);
	if (meshIt == m_spriteMeshes.end()
		|| meshIt->second.w != w || meshIt->second.h != h
		|| meshIt->second.u0 != u0 || meshIt->second.v0 != v0
		|| meshIt->second.u1 != u1 || meshIt->second.v1 != v1)
	{
		if (auto vb = effect->getVertexBuffer<PosTexIndex>())
		{
			vb->setVertices(MakeSpriteVertices(w, h, u0, v0, u1, v1));
			vb->Init();
		}
		m_spriteMeshes[meshKey] = { w, h, u0, v0, u1, v1 };
	}

	DrawSprite(effect, x, y, w, h, scaleX, scaleY, rotationDeg);
}

void UIRenderContext::DrawSprite(const std::shared_ptr<Effect>& effect, float x, float y, float w, float h,
	float scaleX, float scaleY, float rotationDeg)
{
	if (!effect || !m_uiCamera)
	{
		return;
	}

	const float pivotX = x + w * 0.5f;
	const float pivotY = y + h * 0.5f;
	XMMATRIX model = XMMatrixTranslation(pivotX, pivotY, 0.0f)
		* XMMatrixRotationZ(XMConvertToRadians(rotationDeg))
		* XMMatrixScaling(scaleX, scaleY, 1.0f)
		* XMMatrixTranslation(-w * 0.5f, -h * 0.5f, 0.0f);

	auto* dataPtr = effect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtr->model = XMMatrixTranspose(model);
	dataPtr->view = XMMatrixTranspose(m_uiCamera->GetViewXM());
	dataPtr->projection = XMMatrixTranspose(m_uiCamera->GetProjXM());
	dataPtr->TexIndex = 0;
	effect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	effect->apply();
}

void UIRenderContext::ClearFrame()
{
	static float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	auto& d3d = D3DManager::getInstance();
	d3d.clearRenderTarget(white);
	d3d.clearDepthStencil();
}

void UIRenderContext::Present()
{
	D3DManager::getInstance().present(true);
}

ID2D1RenderTarget* UIRenderContext::GetD2DTarget() const
{
	return D3DManager::getInstance().GetD2DRenderTarget();
}

ID2D1SolidColorBrush* UIRenderContext::GetBrush() const
{
	return D3DManager::getInstance().GetColorBrush();
}

IDWriteTextFormat* UIRenderContext::GetTextFormat() const
{
	return D3DManager::getInstance().GetTextFormat();
}
