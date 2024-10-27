#pragma once
#include "EffectComponent.h"

class PSEffectComponent : public EffectComponent {
public:
    PSEffectComponent(std::wstring psFileName, std::wstring csoFileName):m_psFileName(psFileName), m_csoFileName(csoFileName) {}
    ~PSEffectComponent() {};


    virtual void Init();

    virtual void apply();

    void SetPSFileName(std::wstring psFileName) { m_psFileName = psFileName; };
    void SetCSOFileName(std::wstring csoFileName) { m_csoFileName = csoFileName; };

    std::wstring m_psFileName;
    std::wstring m_csoFileName;

    ComPtr<ID3D11PixelShader> m_pPixelShader;
};