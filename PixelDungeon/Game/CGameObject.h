#pragma once
#include "CObj.h"
class CGameObject abstract:
    public CObj
{
public:
    CGameObject();
    virtual ~CGameObject();
public:
    virtual void Initialize() PURE;
    virtual void Late_Update() PURE;
    virtual void Render() PURE;
    virtual void Release() override;
};

