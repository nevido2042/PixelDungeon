#pragma once
#include "CScene.h"
class CMyMenu :
    public CScene
{
    // CScene��(��) ���� ��ӵ�
    void Initialize() override;
    void Update() override;
    void Late_Update() override;
    void Render() override;
    void Release() override;
};

