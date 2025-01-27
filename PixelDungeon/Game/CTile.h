#pragma once
#include "CGameObject.h"
class CTile :
    public CGameObject
{
public:
    CTile();
    virtual ~CTile();
public:
    void Initialize() override;
    void Late_Update() override;
    void Render() override;
public:
    void Set_DrawID(BYTE _byDrawID) { m_byDrawID = _byDrawID; }
    void Set_Option(BYTE _byOption) { m_byOption = _byOption; }

private:
    BYTE    m_byDrawID;
    BYTE    m_byOption;
};

