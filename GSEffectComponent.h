#pragma once
#include "EffectComponent.h"

class GSEffectComponent : public EffectComponent {
public:
    GSEffectComponent(std::wstring psFileName, std::wstring csoFileName) :m_gsFileName(psFileName), m_csoFileName(csoFileName) {}
    ~GSEffectComponent() {};


    virtual void Init();

    virtual void apply();


    void SetGSFileName(std::wstring gsFileName) { m_gsFileName = gsFileName; };
    void SetCSOFileName(std::wstring csoFileName) { m_csoFileName = csoFileName; };

    std::wstring m_gsFileName;
    std::wstring m_csoFileName;


    ComPtr<ID3D11GeometryShader> m_pGeometryShader;
};