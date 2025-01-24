#pragma once
#include "CGameObject.h"
class CPlayer :
    public CGameObject
{
public:
    CPlayer();
    virtual ~CPlayer();
public:
    void Initialize() override;
    int Update() override;
    void Late_Update() override;
    void Render() override;
    void Release() override;
};

