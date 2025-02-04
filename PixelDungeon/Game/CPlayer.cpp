#include "pch.h"
#include "CPlayer.h"
#include "CDevice.h"
#include "CTextureMgr.h"

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
   
    m_eCurState = IDLE;
    m_iFrame = 0;
    m_tInfo.vPos = { 0.f, 0.f, 0.f };
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
   
    /*
D3DXMATRIX	matWorld, matScale, matTrans;

    D3DXMatrixIdentity(&matWorld);
    D3DXMatrixScaling(&matScale, 3.f, 3.f, 3.f);
    D3DXMatrixTranslation(&matTrans,
        m_tInfo.vPos.x,
        m_tInfo.vPos.y,
        m_tInfo.vPos.z);

    m_tInfo.matWorld = matScale * matTrans;

    CDevice::Get_Instance()->Get_Sprite()->SetTransform(&m_tInfo.matWorld);*/
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
    {
        Set_State(WALK);
        Mouse_Update(); 
    }
    else
    {
       // Set_State(IDLE);
    }


    return CObj::Update();
}

void CPlayer::Late_Update()
{
    
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
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
    {
        m_tInfo.vDir = Get_Mouse() - m_tInfo.vPos;
        D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);

        m_tInfo.vPos += m_tInfo.vDir * m_fSpeed;

        // 현재 상태의 최대 프레임 개수를 가져옴
        int iMaxFrame = CTextureMgr::Get_Instance()->Get_TextureCount(L"Player", L"WALK");

        if (iMaxFrame > 0)  // 벡터가 비어있지 않은 경우에만 프레임 증가
        {
            m_iFrame++;
            if (m_iFrame >= iMaxFrame)
                m_iFrame = 0;  // 최대 프레임 개수를 초과하면 다시 0으로
        }
    }
}

