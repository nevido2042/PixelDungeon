#include "pch.h"
#include "CPlayer.h"
#include "CDevice.h"
#include "CTextureMgr.h"
#include "CAstarMgr.h"
#include "TimeMgr.h"


CPlayer::CPlayer()
    :m_eCurState(END), m_iFrame(0), m_fSpeed(0.f)
{
}

CPlayer::~CPlayer()
{
    Release();
}

void CPlayer::Initialize()
{
   
    m_eCurState = WALK;
    m_iFrame = 0;
    m_tInfo.vPos = { 80.f, 80.f, 0.f };
    m_fSpeed = 2.f;
    m_tInfo.vLook = { 1.f, 0.f, 0.f };

    //if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(
    //    L"../Resources/Player/Warrior/warrior%d.png",  // 파일 패턴
    //    TEX_MULTI,
    //    L"Player",    
    //    L"IDLE",      
    //    2)))          
    //{
    //    ERR_MSG(L"Player Idle Texture Insert Failed");
    //    return;
    //}

  
    if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(
        L"../Resources/Player/Warrior/warrior%d.png",
        TEX_MULTI,
        L"Player",
        L"WALK",
        6)))    
        
    {
        ERR_MSG(L"Player Walk Texture Insert Failed");
        return;
    }
}

int CPlayer::Update()
{
    if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && m_eCurState == WALK)
    {
        Set_State(WALK);

        CAstarMgr::Get_Instance()->Start_Astar(m_tInfo.vPos, Get_Mouse());
    }

    return CObj::Update();
}



void CPlayer::Late_Update()
{
    

    Move_Astar();
}



void CPlayer::Render()
{
   
    const TEXINFO* pTexInfo = nullptr;

    if (m_eCurState == IDLE)
    {
       
       // pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Player", L"IDLE", m_iFrame);
    }
    else if (m_eCurState == WALK)
    {
      
        pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Player", L"WALK", m_iFrame);
    }

    if (!pTexInfo)
        return;


    float fCenterX = pTexInfo->tImgInfo.Width / 2.f;
    float fCenterY = pTexInfo->tImgInfo.Height / 2.f;
    D3DXVECTOR3 vCenter(fCenterX, fCenterY, 0.f);

    CDevice::Get_Instance()->Get_Sprite()->SetTransform(&m_tInfo.matWorld);

    CDevice::Get_Instance()->Get_Sprite()->Draw(
        pTexInfo->pTexture,
        nullptr,
        &vCenter,
        nullptr,
        D3DCOLOR_ARGB(255, 255, 255, 255));

   
    
}

void CPlayer::Release()
{
    
}




void CPlayer::Mouse_Update()
{
    if (!CAstarMgr::Get_Instance()->Get_BestList().empty())
        if (!CAstarMgr::Get_Instance()->Get_BestList().empty())
        {
            if (CAstarMgr::Get_Instance()->Get_BestList().size() == 1)
            {
                CAstarMgr::Get_Instance()->Get_BestList().clear(); // 마지막 타일 도착하면 이동 허용
            }
            return;
        }


    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
    {
        m_tInfo.vDir = Get_Mouse() - m_tInfo.vPos;
        D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);

        m_tInfo.vPos += m_tInfo.vDir * m_fSpeed;

        int iMaxFrame = CTextureMgr::Get_Instance()->Get_TextureCount(L"Player", L"WALK");

        if (iMaxFrame > 0)
        {
            m_iFrame++;
            if (m_iFrame >= iMaxFrame)
                m_iFrame = 0;
        }
    }
}

void CPlayer::Move_Astar()
{
    list<TILE*>& BestList = CAstarMgr::Get_Instance()->Get_BestList();

    if (!BestList.empty() && m_eCurState == WALK)
    {
        D3DXVECTOR3 vDir = BestList.front()->vPos - m_tInfo.vPos;
        float fDistance = D3DXVec3Length(&vDir);

        D3DXVec3Normalize(&vDir, &vDir);
        float fMoveSpeed = min(300.f * CTimeMgr::Get_Instance()->Get_TimeDelta(), fDistance);
        m_tInfo.vPos += vDir * fMoveSpeed;

        if (3.f >= fDistance) // 목표 타일 도착 시
        {
            BestList.pop_front();
            if (BestList.empty())  // 모든 타일을 지나면 이동 종료
            {
                //Set_State(IDLE);
            }
        }
    }
}