#pragma once
#include "CScene.h"
class CGameScene :
    public CScene
{
public:
    CGameScene();
    virtual ~CGameScene();
public:
    void Initialize() override;
    void Update() override;
    void Late_Update() override;
    void Render() override;
    void Release() override;
};

