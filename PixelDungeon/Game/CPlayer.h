#pragma once
#include "CGameObject.h"

enum PLAYER_STATE { IDLE, WALK };

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

    void Mouse_Update();
public:
    void Set_State(PLAYER_STATE eState)
    {
        if (m_eCurState != eState)
        {
            m_eCurState = eState;
         
        }
    }


    PLAYER_STATE m_eCurState;
    int m_iFrame;


    D3DXVECTOR3 m_vMouse;
    
};

