#pragma once
#include "CUI.h"
class CGameTitle :
    public CUI
{
public:
    CGameTitle();
    virtual ~CGameTitle();
public:
    void Initialize() override;
    void Late_Update() override;
    void Render() override;
    void Release() override;
};

