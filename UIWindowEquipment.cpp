// UIWindowEquipment.cpp
#include "UIWindowEquipment.h"
using namespace DirectX;

UIWindowEquipment::UIWindowEquipment(HINSTANCE _hInstance) : UIWindow(_hInstance)
{
    windowTitle = L"装配";
}

bool UIWindowEquipment::Init()
{
    // 初始化窗口基础组件
    height = 700;
    width = 700;
    windowTitle = L"装配";
    InitWindowComponent();
    m_windowEffect->Init();

    // 初始化装备槽
    InitSlots();

    // 初始化状态栏
    InitStatusBars();

    InitStatusEffects();

    // 抗性进度条效果
    std::string resistanceTextures[] = {
        "demoTex\\UI\\Window\\em_resist.dds",  // 电磁-蓝
        "demoTex\\UI\\Window\\therm_resist.dds", // 热能-红
        "demoTex\\UI\\Window\\kinetic_resist.dds", // 动能-白
        "demoTex\\UI\\Window\\explosive_resist.dds" // 爆炸-黄
    };

    for (int i = 0; i < 4; ++i) {
        std::vector<PosTexIndex> verts;
        GenerateRectVertex(verts, 0, 0, 50, 16, 0);
        m_resistanceEffects[i] = std::make_shared<Effect>();
        m_resistanceEffects[i]->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\UI\\Process_VS.hlsl", L"HLSL\\UI\\Process_VS.cso");
        m_resistanceEffects[i]->addPixelShader(L"HLSL\\UI\\Process_PS.hlsl", L"HLSL\\UI\\Process_PS.cso");
        m_resistanceEffects[i]->addTextures({ resistanceTextures[i] });
        m_resistanceEffects[i]->getVertexBuffer<PosTexIndex>()->setVertices(verts);
        m_resistanceEffects[i]->addConstantBuffer<ConstantMVPIndex>();
        m_resistanceEffects[i]->Init();
    }

    // CPU/能量进度条
    std::vector<PosTexIndex> cpuEnergyVerts;
    GenerateRectVertex(cpuEnergyVerts, 0, 0, 110, 24, 0);
    m_cpuEnergyEffect = std::make_shared<Effect>();
    m_cpuEnergyEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\UI\\Process_VS.hlsl", L"HLSL\\UI\\Process_VS.cso");
    m_cpuEnergyEffect->addPixelShader(L"HLSL\\UI\\Process_PS.hlsl", L"HLSL\\UI\\Process_PS.cso");
    m_cpuEnergyEffect->addTextures({ "demoTex\\UI\\Window\\cpu_energy_bar.dds" });
    m_cpuEnergyEffect->getVertexBuffer<PosTexIndex>()->setVertices(cpuEnergyVerts);
    m_cpuEnergyEffect->addConstantBuffer<ConstantMVPIndex>();
    m_cpuEnergyEffect->Init();

    std::vector<PosTexIndex> borderVerts;
    GenerateRectVertex(borderVerts, 0, 0, 110, 24, 0);
    m_borderEffect = std::make_shared<Effect>();
    m_borderEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
    m_borderEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
    m_borderEffect->addTextures({ "demoTex\\UI\\Window\\fittingStencil.dds" });
    m_borderEffect->getVertexBuffer<PosTexIndex>()->setVertices(borderVerts);
    m_borderEffect->addConstantBuffer<ConstantMVPIndex>();
    m_borderEffect->Init();

    std::vector<PosTexIndex> borderVertsResistance;
    GenerateRectVertex(borderVertsResistance, 0, 0, 50, 16, 0);
    m_borderEffectResistance = std::make_shared<Effect>();
    m_borderEffectResistance->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
    m_borderEffectResistance->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
    m_borderEffectResistance->addTextures({ "demoTex\\UI\\Window\\fittingStencil.dds" });
    m_borderEffectResistance->getVertexBuffer<PosTexIndex>()->setVertices(borderVertsResistance);
    m_borderEffectResistance->addConstantBuffer<ConstantMVPIndex>();
    m_borderEffectResistance->Init();

    for (auto& component : childComponents) {
        component->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
        component->Init();
    }
    return true;
}

void UIWindowEquipment::InitSlots()
{
    int index = 0;
    auto text_h = std::make_shared<UIText>();
    text_h->setSize(35, 65 + 36.0f * index++, 350.0f, 350.0f);
    text_h->setText(L"高能量槽");
    m_UITexts.push_back(text_h);
    m_slots.push_back(InitSlot(HIGH_SLOT, index++, 482));
    m_slots.push_back(InitSlot(HIGH_SLOT, index++, 482));
    m_slots.push_back(InitSlot(HIGH_SLOT, index++, 0)); ;
    auto text_m = std::make_shared<UIText>();
    text_m->setSize(35, 65 + 36.0f * index++, 350.0f, 350.0f);
    text_m->setText(L"中能量槽");
    m_UITexts.push_back(text_m);
    m_slots.push_back(InitSlot(MED_SLOT, index++, 439));
    m_slots.push_back(InitSlot(MED_SLOT, index++, 0));
    m_slots.push_back(InitSlot(MED_SLOT, index++, 0));
    m_slots.push_back(InitSlot(MED_SLOT, index++, 0));
    auto text_l = std::make_shared<UIText>();
    text_l->setSize(35, 65 + 36.0f * index++, 350.0f, 350.0f);
    text_l->setText(L"低能量槽");
    m_UITexts.push_back(text_l);
    m_slots.push_back(InitSlot(LOW_SLOT, index++, 22542));
    m_slots.push_back(InitSlot(LOW_SLOT, index++, 0));
    m_slots.push_back(InitSlot(LOW_SLOT, index++, 0));
    auto text_r = std::make_shared<UIText>();
    text_r->setSize(35, 65 + 36.0f * index++, 350.0f, 350.0f);
    text_r->setText(L"改装件槽");
    m_UITexts.push_back(text_r);
    m_slots.push_back(InitSlot(RIG_SLOT, index++, 26024));
    m_slots.push_back(InitSlot(RIG_SLOT, index++, 0));
    m_slots.push_back(InitSlot(RIG_SLOT, index++, 0));

    for (auto& text : m_UITexts) {
        text->switchTextFormat("Arial_XS");
        text->Init();
    }
}

UIWindowEquipment::EquipmentSlot UIWindowEquipment::InitSlot(SlotType slotType, int index, int typeID)
{
    std::string texturePath = "demoTex\\EVE\\media\\res\\Uprising_V21.03_Icons\\Icons\\UI\\Fitting\\dds\\";
    

    EquipmentSlot temp;
    temp.index = index;
    temp.type = slotType;
    temp.typeID = typeID;
    temp.itemName = std::make_shared<UIText>();
    temp.itemName->setSize(80, 65 + 36.0f * index, 350.0f, 350.0f);

    if (slotType == HIGH_SLOT)
    {
        temp.itemName->setText(L"HIGH_SLOT");
        texturePath += "filterIconHighSlot_32.dds";
    }
    if (slotType == MED_SLOT)
    {
        temp.itemName->setText(L"MED_SLOT");
        texturePath += "filterIconMediumSlot_32.dds";
    }
    if (slotType == LOW_SLOT)
    {
        temp.itemName->setText(L"LOW_SLOT");
        texturePath += "filterIconLowSlot_32.dds";
    }
    if (slotType == RIG_SLOT)
    {
        temp.itemName->setText(L"RIG_SLOT");
        texturePath += "filterIconRigSlot.dds";
    }
    std::vector<PosTexIndex> Verts;
    if (typeID == 0) {
        GenerateRectVertex(Verts, 44, 64 + 36.0f * index, 24, 24, 0);
        temp.slotEffect = std::make_shared<Effect>();
        temp.slotEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
        temp.slotEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
        temp.slotEffect->addTextures({ texturePath });
        temp.slotEffect->getVertexBuffer<PosTexIndex>()->setVertices(Verts);
        temp.slotEffect->addConstantBuffer<ConstantMVPIndex>();
        temp.slotEffect->Init();
    }
    else {
        GenerateRectVertex(Verts, 40, 60 + 36.0f * index, 32, 32, 0);
        temp.itemEffect = std::make_shared<Effect>();
        temp.itemEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
        temp.itemEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
        temp.itemEffect->addTextures({ getIconPathByTypeID(typeID) });
        temp.itemEffect->getVertexBuffer<PosTexIndex>()->setVertices(Verts);
        temp.itemEffect->addConstantBuffer<ConstantMVPIndex>();
        temp.itemEffect->Init();
        temp.itemName->setText(InvTypesManager::getInstance()->getNameByTypeId(typeID));
    }
    temp.itemName->Init();
    return temp;
}

void UIWindowEquipment::InitStatusBars()
{
    auto addStatusText = [&](const XMFLOAT2& pos, const std::wstring& text) {
        auto textComp = std::make_shared<UIText>();
        textComp->setSize(pos.x, pos.y, 200, 30);
        textComp->setText(text);
        textComp->Init();
        m_statusTexts.push_back(textComp);
    };

    addStatusText({ 425, 84 }, L"52.1/135.0 tf");
    addStatusText({ 555, 84 }, L"78.3/150.0 MW");
    addStatusText({ 400, 40 }, L"装配资源");
    addStatusText({ 400, 115 }, L"抗性");
    addStatusText({ 430, 140 - 2 }, L"15%");
    addStatusText({ 430, 170 - 2 }, L"55%");
    addStatusText({ 430, 200 - 2 }, L"33%");
    addStatusText({ 490, 140 - 2 }, L"25%");
    addStatusText({ 490, 170 - 2 }, L"45%");
    addStatusText({ 490, 200 - 2 }, L"33%");
    addStatusText({ 550, 140 - 2 }, L"35%");
    addStatusText({ 550, 170 - 2 }, L"35%");
    addStatusText({ 550, 200 - 2 }, L"33%");
    addStatusText({ 610, 140 - 2 }, L"45%");
    addStatusText({ 610, 170 - 2 }, L"25%");
    addStatusText({ 610, 200 - 2 }, L"33%");
    addStatusText({ 400, 240 }, L"电容");
    addStatusText({ 425, 270 }, L"总计：3.78kGJ");
    addStatusText({ 425, 290 }, L"稳定：49.1%");
    addStatusText({ 555, 280 }, L"+8.23GJ/s");

    float offset = 20;
    addStatusText({ 400, 320 + offset }, L"目标和导航");

    addStatusText({ 405, 350 + offset }, L"最大锁定数:3");
    addStatusText({ 405, 375 + offset }, L"锁定范围:  17 km");
    addStatusText({ 405, 400 + offset }, L"扫描精度:  368 mm");
    addStatusText({ 405, 425 + offset }, L"传感器强度:27.6");

    addStatusText({ 540, 350 + offset }, L"巡航速度:  219 m/s");
    addStatusText({ 540, 375 + offset }, L"起跳时间:  3.1s");
    addStatusText({ 540, 400 + offset }, L"信号半径:  4 m");
    addStatusText({ 540, 425 + offset }, L"跃迁速度:  3 AU/s");
}

void UIWindowEquipment::InitStatusEffects()
{
    InitStatusEffect(3887, 395, 80);
    InitStatusEffect(1353, 530, 80);
    InitStatusEffect(2545, 395, 140);
    InitStatusEffect(1284, 395, 170);
    InitStatusEffect(2603, 395, 200);
    InitStatusEffect(2018, 395, 280);
    InitStatusEffect(1195, 530, 280);
}

void UIWindowEquipment::InitStatusEffect(int typeID, int _x, int _y)
{
    std::vector<PosTexIndex> statusVerts;
    std::shared_ptr<Effect> m_statusEffect = std::make_shared<Effect>();
    GenerateRectVertex(statusVerts, _x, _y, 24, 24, 1.0f);
    m_statusEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\UI\\Process_VS.hlsl", L"HLSL\\UI\\Process_VS.cso");
    m_statusEffect->addPixelShader(L"HLSL\\UI\\Process_PS.hlsl", L"HLSL\\UI\\Process_PS.cso");
    m_statusEffect->addTextures({
        getIconPathByTypeID(typeID),
        });
    m_statusEffect->getVertexBuffer<PosTexIndex>()->setVertices(statusVerts);
    m_statusEffect->addConstantBuffer<ConstantMVPIndex>();
    m_statusEffect->Init();
    m_statusEffects.push_back(m_statusEffect);
}

void UIWindowEquipment::DrawUI()
{
    UIWindow::DrawUI();

    XMMATRIX view = m_pUICamera->GetViewXM();
    XMMATRIX proj = m_pUICamera->GetProjXM();

    XMMATRIX model = XMMatrixTranslation(x, y, 0);
    ConstantMVPIndex* dataPtr = m_windowEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
    dataPtr->model = XMMatrixTranspose(model);
    dataPtr->view = XMMatrixTranspose(view);
    dataPtr->projection = XMMatrixTranspose(proj);
    dataPtr->TexIndex = 0;
    m_windowEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
    m_windowEffect->apply();

    for(auto & m_statusEffect: m_statusEffects)
    {
        dataPtr = m_statusEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
        dataPtr->model = XMMatrixTranspose(model);
        dataPtr->view = XMMatrixTranspose(view);
        dataPtr->projection = XMMatrixTranspose(proj);
        dataPtr->TexIndex = 0;
        m_statusEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
        m_statusEffect->apply();
    }

    DrawFractionBar(m_cpuEnergyEffect, 420, 80, 52.1f / 135.0f);
    DrawFractionBar(m_cpuEnergyEffect, 550, 80, 78.3f / 150.0f);
    DrawFractionBarBorder(m_borderEffect, 420, 80);
    DrawFractionBarBorder(m_borderEffect, 550, 80);

    for (int i = 0; i < 4; ++i) {
        DrawFractionBar(m_resistanceEffects[i], 420 + i * 60, 140, 0.15f + 0.10f * i);
        DrawFractionBarBorder(m_borderEffectResistance, 420 + i * 60, 140);
        DrawFractionBar(m_resistanceEffects[i], 420 + i * 60, 170, 0.55f - 0.05f * i);
        DrawFractionBarBorder(m_borderEffectResistance, 420 + i * 60, 170);
        DrawFractionBar(m_resistanceEffects[i], 420 + i * 60, 200, 0.33f);
        DrawFractionBarBorder(m_borderEffectResistance, 420 + i * 60, 200);

    }

    // 绘制装备槽
    for (auto& slot : m_slots) {
        
        if (slot.typeID != 0) {
            ConstantMVPIndex* dataPtr = slot.itemEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
            dataPtr->model = XMMatrixTranspose(model);
            dataPtr->view = XMMatrixTranspose(view);
            dataPtr->projection = XMMatrixTranspose(proj);
            dataPtr->TexIndex = 0;
            slot.itemEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
            slot.itemEffect->apply();
        }
        else {
            ConstantMVPIndex* dataPtr = slot.slotEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
            dataPtr->model = XMMatrixTranspose(model);
            dataPtr->view = XMMatrixTranspose(view);
            dataPtr->projection = XMMatrixTranspose(proj);
            dataPtr->TexIndex = 0;
            slot.slotEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
            slot.slotEffect->apply();
        }
        slot.itemName->DrawUI();
    }
    // 绘制右边状态栏
    // CPU/能量
    // 抗性进度条
    /*const XMFLOAT2 resistPos[4] = { {450,200}, {450,230}, {450,260}, {450,290} };
    for (int i = 0; i < 4; ++i) {
        DrawProgressBar(resistPos[i], 120, 20, m_resistances[i], m_resistanceEffects[i]);
    }*/

    // 绘制文本
    for (auto& text : m_statusTexts) {
        text->DrawUI();
    }


    for (auto& text : m_UITexts) {
        text->DrawUI();
    }

    for (auto& component : childComponents) {
        component->DrawUI();
    }
}

void UIWindowEquipment::DrawProgressBar(const XMFLOAT2& pos, float width, float height, float progress, const std::shared_ptr<Effect>& effect)
{
    XMMATRIX model = XMMatrixScaling(progress, 1.0f, 1.0f) *
        XMMatrixTranslation(pos.x, pos.y, 0);

    ConstantMVPIndex* cb = effect->getConstantBuffer<ConstantMVPIndex>()->Map();
    cb->model = XMMatrixTranspose(model);
    cb->view = XMMatrixTranspose(m_pUICamera->GetViewXM());
    cb->projection = XMMatrixTranspose(m_pUICamera->GetProjXM());
    cb->TexIndex = 0.5f;
    effect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
    effect->apply();
}

void UIWindowEquipment::DrawFractionBar(const std::shared_ptr<Effect>& effect, int dx, int dy, float progress)
{
    XMMATRIX model = XMMatrixTranslation(x + dx, y + dy, 0);
    ConstantMVPIndex* dataPtr = effect->getConstantBuffer<ConstantMVPIndex>()->Map();
    dataPtr->model = XMMatrixTranspose(model);
    dataPtr->view = XMMatrixTranspose(m_pUICamera->GetViewXM());
    dataPtr->projection = XMMatrixTranspose(m_pUICamera->GetProjXM());
    dataPtr->TexIndex = progress;
    effect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
    effect->apply();

}

void UIWindowEquipment::DrawFractionBarBorder(const std::shared_ptr<Effect>& effect, int dx, int dy)
{
    XMMATRIX model = XMMatrixTranslation(x + dx, y + dy, 0);
    ConstantMVPIndex* dataPtr = effect->getConstantBuffer<ConstantMVPIndex>()->Map();
    dataPtr->model = XMMatrixTranspose(model);
    dataPtr->view = XMMatrixTranspose(m_pUICamera->GetViewXM());
    dataPtr->projection = XMMatrixTranspose(m_pUICamera->GetProjXM());
    dataPtr->TexIndex = 0.0f;
    effect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
    effect->apply();
}

void UIWindowEquipment::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
    UIWindow::UpdateUI(dt, mouse, keyboard, tick);

    for (auto& text : m_statusTexts) {
        text->setDelta(x,y);
    }

    for (auto& text : m_UITexts) {
        text->setDelta(x, y);
    }

    for (auto& slot : m_slots) {
        slot.itemName->setDelta(x, y);
    }
}