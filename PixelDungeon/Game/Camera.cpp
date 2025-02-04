#include "pch.h"
#include "Camera.h"
#include "CObjMgr.h"
#include "CKeyMgr.h"
#include "CDevice.h"

CCamera* CCamera::m_pInstance = nullptr;

CCamera::CCamera()
    :m_fHeight(0.f), m_fWidth(0.f), m_fZoom(0.f),
    m_fViewHeight(0.f), m_fViewWidth(0.f), m_fMoveX(0.f), m_fMoveY(0.f), m_bMoveLerp(false),
    m_bIsShaking(false), m_fShakeDuration(0.f), m_fShakeIntensity(0.f), m_fShakeTimer(0.f)
{

}

CCamera::~CCamera()
{
}

void CCamera::Move_To_Lerp(float _fX, float _fY)
{
    m_fMoveX = _fX;
    m_fMoveY = _fY;
    m_bMoveLerp = true;
}

void CCamera::Initialize()
{
    m_fWidth = WINCX;
    m_fHeight = WINCY;

    m_tInfo.vPos.x = m_fWidth * 0.5f;
    m_tInfo.vPos.y = m_fHeight * 0.5f;

    m_fZoom = 1.f;
}

void CCamera::Update()
{
    float fSpeed(10.f);
    if (CKeyMgr::Get_Instance()->Key_Pressing('A'))
    {
        m_bMoveLerp = false;
        m_tInfo.vPos.x -= fSpeed;
    }
    if (CKeyMgr::Get_Instance()->Key_Pressing('D'))
    {
        m_bMoveLerp = false;
        m_tInfo.vPos.x += fSpeed;
    }
    if (CKeyMgr::Get_Instance()->Key_Pressing('W'))
    {
        m_bMoveLerp = false;
        m_tInfo.vPos.y -= fSpeed;
    }
    if (CKeyMgr::Get_Instance()->Key_Pressing('S'))
    {
        m_bMoveLerp = false;
        m_tInfo.vPos.y += fSpeed;
    }

    // ȭ�� ũ��
    //int screenWidth = WINCX;
    //int screenHeight = WINCY;

    if (CKeyMgr::Get_Instance()->Key_Pressing(VK_ADD)) // ����
    {
        m_fZoom += 0.01f;
        if (m_fZoom > ZOOM_MAX) m_fZoom = ZOOM_MAX; // �ִ� �� ����
    }

    if (CKeyMgr::Get_Instance()->Key_Pressing(VK_SUBTRACT)) // �ܾƿ�
    {
        m_fZoom -= 0.01f;
        if (m_fZoom < ZOOM_MIN) m_fZoom = ZOOM_MIN; // �ּ� �� ����
    }

    if (CKeyMgr::Get_Instance()->Key_Pressing(VK_NUMPAD0)) // �� ���� �⺻
    {
        m_fZoom = 1.f;
    }




    // ����ŷ ó��
    if (m_bIsShaking)
    {
        float finalX = m_tInfo.vPos.x;
        float finalY = m_tInfo.vPos.y;

        //m_fShakeTimer += GAMESPEED;

        // ����ŷ�� ���ӵǴ� ����
        if (m_fShakeTimer < m_fShakeDuration)
        {
            // �������� �̿��� ����ŷ
            float offsetX = (rand() % 100 / 100.0f - 0.5f) * m_fShakeIntensity * 2.0f;
            float offsetY = (rand() % 100 / 100.0f - 0.5f) * m_fShakeIntensity * 2.0f;

            finalX += offsetX;
            finalY += offsetY;
        }
        else
        {
            // ����ŷ ����
            m_bIsShaking = false;
            m_fShakeTimer = 0.0f;
        }

        // ī�޶� ���� ��ġ ����
        m_tInfo.vPos.x = finalX;
        m_tInfo.vPos.y = finalY;
    }

}

void CCamera::Late_Update()
{
    if (m_bMoveLerp)
    {
        // ��ǥ ��ġ ���
        float fTargetX = m_fMoveX;// -m_fMoveX + WINCX * 0.5f;
        float fTargetY = m_fMoveY;// -m_fMoveY + WINCY * 0.5f;

        // ���� ��ġ ��������
        float fCurrentX = Get_X();
        float fCurrentY = Get_Y();

        // ���� ���� ���� (0.0f ~ 1.0f)
        float fLerpFactor = 0.1f; // 0.1f = ������ �̵�, 1.0f = ��� �̵�

        // LERP ���
        float fNewX = fCurrentX + (fTargetX - fCurrentX) * fLerpFactor;
        float fNewY = fCurrentY + (fTargetY - fCurrentY) * fLerpFactor;

        // ���ο� ��ġ ����
        Set_Pos(fNewX, fNewY);

        // �Ÿ� ��� (��Ŭ���� �Ÿ�)
        float fDeltaX = fTargetX - fCurrentX;
        float fDeltaY = fTargetY - fCurrentY;
        float fDistance = sqrt(fDeltaX * fDeltaX + fDeltaY * fDeltaY);

        // ��ǥ ��ġ�� ��������� ����
        if (fDistance < 1.0f) { // 1.0f�� �Ӱ谪(��� ����)���� ���� ����
            m_bMoveLerp = false;
        }
    }
}

bool CCamera::IsInCameraView(float _fX, float _fY)
{
    float fLeft = m_tInfo.vPos.x - (m_fWidth * 0.5f / m_fZoom);
    float fRight = m_tInfo.vPos.x + (m_fWidth * 0.5f / m_fZoom);
    float fTop = m_tInfo.vPos.y - (m_fHeight * 0.5f / m_fZoom);
    float fBottom = m_tInfo.vPos.y + (m_fHeight * 0.5f / m_fZoom);

    m_fViewWidth = abs(fLeft - fRight);
    m_fViewHeight = abs(fTop - fBottom);

    // ��ü�� ī�޶� ����Ʈ�� ��ġ���� Ȯ��
    return !(_fX < fLeft || _fX > fRight || _fY < fTop || _fY > fBottom);
}

void CCamera::Render()
{
    // ���콺 ��ġ
    POINT ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(g_hWnd, &ptMouse);

    // ���콺 ��ǥ�� ���ڿ��� ��ȯ
    wchar_t szBuf[MAX_STR];
    swprintf(szBuf, 100, L"���콺 ��ũ�� ��ǥ:(%d, %d)", ptMouse.x, ptMouse.y);

    // ��Ʈ�� ���콺 ��ǥ ���
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity(&matIdentity);
    CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matIdentity);

    CDevice::Get_Instance()->Get_Font()->DrawTextW(
        CDevice::Get_Instance()->Get_Sprite(),
        szBuf,
        lstrlenW(szBuf),
        nullptr,
        0,
        D3DCOLOR_ARGB(255, 255, 255, 255)
    );

    // ī�޶� ũ�⿡ �´� �簢�� �׵θ� �� �׸���
    DrawCameraBorder();
}

void CCamera::DrawCameraBorder()
{
    // ī�޶��� ũ��� ��ġ�� �����ɴϴ�.
    float camWidth = m_fViewWidth/2;  // ī�޶��� �ʺ�
    float camHeight = m_fViewHeight/2; // ī�޶��� ����
    float camX = m_tInfo.vPos.x;          // ī�޶��� X ��ǥ
    float camY = m_tInfo.vPos.y;          // ī�޶��� Y ��ǥ

    // �簢���� �� ������ ��ǥ ���
    D3DXVECTOR2 topLeft = D3DXVECTOR2(camX - camWidth, camY - camHeight);
    D3DXVECTOR2 topRight = D3DXVECTOR2(camX + camWidth, camY - camHeight);
    D3DXVECTOR2 bottomLeft = D3DXVECTOR2(camX - camWidth, camY + camHeight);
    D3DXVECTOR2 bottomRight = D3DXVECTOR2(camX + camWidth, camY + camHeight);

    // �� ���� (���)
    D3DCOLOR lineColor = D3DCOLOR_ARGB(255, 255, 255, 255);

    // �� �׸���
    CDevice::Get_Instance()->Get_Line()->SetWidth(2.0f); // �� �β� ����
    CDevice::Get_Instance()->Get_Line()->Begin();

    // �簢�� �׵θ� �� �׸���
    D3DXVECTOR2 lines[] = {
        topLeft, topRight,
        topRight, bottomRight,
        bottomRight, bottomLeft,
        bottomLeft, topLeft
    };

    CDevice::Get_Instance()->Get_Line()->Draw(lines, 8
        , lineColor);

    CDevice::Get_Instance()->Get_Line()->End();
}

void CCamera::Start_Shake(float intensity, float duration)
{
    m_bIsShaking = true;
    m_fShakeIntensity = intensity;
    m_fShakeDuration = duration;
    m_fShakeTimer = 0.0f; // Ÿ�̸� �ʱ�ȭ
}
