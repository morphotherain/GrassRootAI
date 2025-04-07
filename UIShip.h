#pragma once
#include <fstream>
#include "UIBase.h"
#include "UIText.h"

class UIShip : public UIBase
{
public:
	UIShip() = default;
	UIShip(HINSTANCE _hInstance) :UIBase(_hInstance) {}
	~UIShip() = default;

	//

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawUI();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
	void setSize(const float _x, const float _y, const float _deltaX, const float _deltaY) { x = _x, y = _y, deltaX = _deltaX, deltaY = _deltaY; }
	void setTex(std::string _Tex) { TexPath = _Tex; };
	void setLocalCamera(std::shared_ptr<Camera> _localCamera) {
		m_pLocalCamera = _localCamera;
	};

	void UpdateLockedTargets(UINT tick);

	void InitCommonEffect(std::vector<PosTexIndex>& _vertices, std::vector<std::string> textureFileName, std::shared_ptr<Effect>& m_effect);
	void DrawCommonEffect(std::shared_ptr<Effect>& m_effect, DirectX::XMMATRIX& windowModel, DirectX::XMMATRIX& viewMatrix, DirectX::XMMATRIX& projMatrix);

	void InitEquipmentEffect(std::shared_ptr<Effect>& m_effect);
	void InitGaugeEffect(std::vector<PosTexIndex>& _vertices, std::vector<std::string> textureFileName, std::shared_ptr<Effect>& m_effect);
	void DrawGaugeEffect(std::shared_ptr<Effect>& m_effect, DirectX::XMMATRIX& windowModel, DirectX::XMMATRIX& viewMatrix, DirectX::XMMATRIX& projMatrix, float hpRadio);

	void GenerateCapacitorDots(std::vector<PosTexIndex>& vertices, int numRadii, float centerX, float centerY, float circleRadius);
	void GenerateSlotVertices(std::vector<PosTexIndex>& vertices, int row, int col, float texID);
	void GenerateSlotEquipmentVertices(std::vector<PosTexIndex>& vertices, int row, int col, float texID);
	void GenerateSlotUnderlayVertices(std::vector<PosTexIndex>& vertices);
	void GenerateSlotOverloadVertices(std::vector<PosTexIndex>& vertices, int row, int col);
	void GenerateSlotOverloadsVertices(std::vector<PosTexIndex>& vertices);

	void GenerateLockedTarget(int objectID);
	void DeleteLockedTarget(int objectID);
	void InitLockedTargetGaugeEffect(std::vector<PosTexIndex>& _vertices, std::vector<std::string> textureFileName, std::shared_ptr<Effect>& m_effect);

	void DrawLockedTarget();

	struct LockedTarget {
		LockedTarget() = default;
		~LockedTarget() = default;
		float x = 0.0f;
		float y = 0.0f;
		float target_x = 0.0f;
		float target_y = 0.0f;

		float m_lockedProcess = 0.0f;
		bool m_isLocked = false;
		bool m_checked = false;

		std::shared_ptr<Effect> m_gaugeShieldEffect;
		std::shared_ptr<Effect> m_gaugeArmorEffect;
		std::shared_ptr<Effect> m_gaugeStructureEffect;

		std::shared_ptr<Effect> m_gaugeRedEffect;
		std::shared_ptr<Effect> m_bodyEffect;
		std::shared_ptr<Effect> m_typeImgEffect;
		std::shared_ptr<Effect> m_arrowEffect;

		std::shared_ptr<Effect> m_lockedCircleEffect;
		std::shared_ptr<Effect> m_lockedLineEffect;
		std::shared_ptr<Effect> m_lockedarrowEffect;

		std::shared_ptr<Effect> m_lockingCircleEffect;

		std::shared_ptr<UIText> m_lockedObjectName;
	};

private:
	std::fstream fs;
	std::shared_ptr <Camera> m_pLocalCamera;

	float x = 10.0f;
	float y = 30.0f;
	float deltaX = 20.0f;
	float deltaY = 4.0f;
	std::string TexPath;
	std::string front = "demoTex\\EVE\\media\\res\\Uprising_V21.03_Icons\\Icons\\UI\\ShipUI\\dds\\";

	int m_tick = 0;

	std::shared_ptr<Effect> m_mainUnderlayEffect;
	std::shared_ptr<Effect> m_speedoUnderlayEffect;

	std::shared_ptr<Effect> m_gaugeRedEffect;
	std::shared_ptr<Effect> m_gaugeShieldEffect;
	std::shared_ptr<Effect> m_gaugeArmorEffect;
	std::shared_ptr<Effect> m_gaugeStructureEffect;

	std::shared_ptr<Effect> m_lowHighHeatEffect;
	std::shared_ptr<Effect> m_medHeatEffect;

	std::shared_ptr<Effect> m_plusMinusEffect;

	std::shared_ptr<Effect> m_overloadBtnEffect;

	std::shared_ptr<Effect> m_slotBaseEffect;
	std::shared_ptr<Effect> m_slotOverloadBtnEffect;
	std::shared_ptr<Effect> m_slotEquipmentEffect;

	std::shared_ptr<Effect> m_capacitorEffect;

	std::unordered_map<int, std::shared_ptr<LockedTarget>> m_mapLockedTarget;
};
