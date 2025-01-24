#pragma once
#include "CScene.h"
class CMyMenu :
    public CScene
{
    // CScene을(를) 통해 상속됨
    void Initialize() override;
    void Update() override;
    void Late_Update() override;
    void Render() override;
    void Release() override;
};

