#pragma once
#include "UIWindow.h"
#include "UIText.h"
#include "UIButton.h"

class UIWindowEquipment : public UIWindow
{
public:
    UIWindowEquipment() :UIWindow() { windowTitle = L"装配"; };
    UIWindowEquipment(HINSTANCE _hInstance);
    ~UIWindowEquipment() = default;

    virtual bool Init() override;
    virtual void DrawUI() override;
    virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick) override;

private:
    // 装备槽类型
    enum SlotType { HIGH_SLOT, MED_SLOT, LOW_SLOT, RIG_SLOT };

    // 装备槽结构
    struct EquipmentSlot {
        int index = 0;
        SlotType type;
        int typeID = 0; // 0表示空槽
        std::shared_ptr<Effect> slotEffect;
        std::shared_ptr<Effect> itemEffect;
        std::shared_ptr<UIText> itemName;
    };

    // 初始化部分
    void InitSlots();
    EquipmentSlot InitSlot(SlotType slotType, int index, int typeID);
    void InitStatusBars();
    void InitStatusEffects();
    void InitStatusEffect(int typeID, int _x ,int _y);


    // 进度条相关
    void DrawProgressBar(const DirectX::XMFLOAT2& pos, float width, float height, float progress, const std::shared_ptr<Effect>& effect);
    void DrawFractionBar(const std::shared_ptr<Effect>& effect, int dx, int dy, float progress);
    void DrawFractionBarBorder(const std::shared_ptr<Effect>& effect, int dx, int dy);

    // 演示数据
    float m_cpuUsage = 52.1f;
    float m_cpuMax = 135.0f;
    float m_energyUsage = 78.3f;
    float m_energyMax = 150.0f;
    float m_resistances[4] = { 0.75f, 0.6f, 0.55f, 0.8f }; // 电磁/热能/动能/爆炸

    // 图形资源
    std::vector<EquipmentSlot> m_slots;
    std::shared_ptr<Effect> m_resistanceEffects[4];
    std::shared_ptr<Effect> m_cpuEnergyEffect;
    std::shared_ptr<Effect> m_borderEffect;
    std::shared_ptr<Effect> m_borderEffectResistance;
    std::vector <std::shared_ptr<Effect>> m_statusEffects;
    std::vector<std::shared_ptr<UIText>> m_statusTexts;
    std::vector<std::shared_ptr<UIText>> m_UITexts;
};