#pragma once
#include "CObj.h"
class CUI abstract:
    public CObj
{
public:
    CUI();
    virtual ~CUI();
public:
    virtual void Initialize() PURE;
    virtual void Late_Update() PURE;
    virtual void Render() PURE;
    virtual void Release() override;
};

