#pragma once

#include "UIRect.h"
#include "D3DManager.h"
#include <memory>
#include <string>
#include <unordered_map>

class OrthographicCamera;
class Effect;

// UIF 渲染上下文：复用 D3DManager + Effect，不重复持有 D3D/D2D 设备指针。
class UIRenderContext
{
public:
	void Initialize(int clientWidth, int clientHeight);
	void OnResize(int clientWidth, int clientHeight);

	int GetClientWidth() const { return m_clientWidth; }
	int GetClientHeight() const { return m_clientHeight; }

	std::shared_ptr<OrthographicCamera> GetUICamera() const { return m_uiCamera; }

	std::shared_ptr<Effect> GetOrCreateSpriteEffect(const std::string& texturePath, float w, float h,
		float u0 = 0.0f, float v0 = 0.0f, float u1 = 1.0f, float v1 = 1.0f);

	void DrawUIQuad(const std::string& texturePath, float x, float y, float w, float h,
		float scaleX = 1.0f, float scaleY = 1.0f, float rotationDeg = 0.0f,
		float u0 = 0.0f, float v0 = 0.0f, float u1 = 1.0f, float v1 = 1.0f);

	void DrawSprite(const std::shared_ptr<Effect>& effect, float x, float y, float w, float h,
		float scaleX = 1.0f, float scaleY = 1.0f, float rotationDeg = 0.0f);

	void ClearFrame();
	void Present();

	ID2D1RenderTarget* GetD2DTarget() const;
	ID2D1SolidColorBrush* GetBrush() const;
	IDWriteTextFormat* GetTextFormat() const;

private:
	struct SpriteMeshState
	{
		float w = 0.0f;
		float h = 0.0f;
		float u0 = 0.0f;
		float v0 = 0.0f;
		float u1 = 1.0f;
		float v1 = 1.0f;
	};

	static std::string MakeSpriteMeshKey(const std::string& texturePath, float w, float h,
		float u0, float v0, float u1, float v1);

	int m_clientWidth = 0;
	int m_clientHeight = 0;

	std::shared_ptr<OrthographicCamera> m_uiCamera;
	std::unordered_map<std::string, std::shared_ptr<Effect>> m_spriteEffects;
	std::unordered_map<std::string, SpriteMeshState> m_spriteMeshes;
};
