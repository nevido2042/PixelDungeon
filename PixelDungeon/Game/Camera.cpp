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

POINT CCamera::WorldToScreen(float worldX, float worldY)
{
    POINT screenPoint;
    screenPoint.x = int((worldX - m_tInfo.vPos.x) * m_fZoom + m_fWidth / 2);
    screenPoint.y = int((worldY - m_tInfo.vPos.y) * m_fZoom + m_fHeight / 2);
    return screenPoint;
}

D3DXMATRIX CCamera::WorldToScreen(const D3DXMATRIX& matWorld)
{
    // Extract the translation (position) components from the matrix
    float worldX = matWorld._41; // Assuming _41 contains the X translation
    float worldY = matWorld._42; // Assuming _42 contains the Y translation

    D3DXMATRIX matRender = matWorld;
    matRender._41 = (worldX - m_tInfo.vPos.x) * m_fZoom + m_fWidth / 2; // Update X position
    matRender._42 = (worldY - m_tInfo.vPos.y) * m_fZoom + m_fHeight / 2; // Update Y position

    return matRender;
}

POINT CCamera::ScreenToWorld(int screenX, int screenY)
{
    POINT worldPoint;
    worldPoint.x = int((screenX - m_fWidth / 2) / m_fZoom + m_tInfo.vPos.x);
    worldPoint.y = int((screenY - m_fHeight / 2) / m_fZoom + m_tInfo.vPos.y);
    return worldPoint;
}

D3DXVECTOR3 CCamera::ScreenToWorld(const D3DXVECTOR3& vecScreen)
{
    D3DXVECTOR3 vecWorld;
    vecWorld.x = (vecScreen.x - m_fWidth / 2) / m_fZoom + m_tInfo.vPos.x; // Convert X
    vecWorld.y = (vecScreen.y - m_fHeight / 2) / m_fZoom + m_tInfo.vPos.y; // Convert Y
    vecWorld.z = vecScreen.z; // Preserve Z as is

    return vecWorld;
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

    // 화면 크기
    //int screenWidth = WINCX;
    //int screenHeight = WINCY;

    if (CKeyMgr::Get_Instance()->Key_Pressing(VK_ADD)) // 줌인
    {
        m_fZoom += 0.01f;
        if (m_fZoom > ZOOM_MAX) m_fZoom = ZOOM_MAX; // 최대 줌 제한
    }

    if (CKeyMgr::Get_Instance()->Key_Pressing(VK_SUBTRACT)) // 줌아웃
    {
        m_fZoom -= 0.01f;
        if (m_fZoom < ZOOM_MIN) m_fZoom = ZOOM_MIN; // 최소 줌 제한
    }

    if (CKeyMgr::Get_Instance()->Key_Pressing(VK_NUMPAD0)) // 줌 배율 기본
    {
        m_fZoom = 1.f;
    }




    // 쉐이킹 처리
    if (m_bIsShaking)
    {
        float finalX = m_tInfo.vPos.x;
        float finalY = m_tInfo.vPos.y;

        //m_fShakeTimer += GAMESPEED;

        // 쉐이킹이 지속되는 동안
        if (m_fShakeTimer < m_fShakeDuration)
        {
            // 랜덤값을 이용한 쉐이킹
            float offsetX = (rand() % 100 / 100.0f - 0.5f) * m_fShakeIntensity * 2.0f;
            float offsetY = (rand() % 100 / 100.0f - 0.5f) * m_fShakeIntensity * 2.0f;

            finalX += offsetX;
            finalY += offsetY;
        }
        else
        {
            // 쉐이킹 종료
            m_bIsShaking = false;
            m_fShakeTimer = 0.0f;
        }

        // 카메라 최종 위치 적용
        m_tInfo.vPos.x = finalX;
        m_tInfo.vPos.y = finalY;
    }

}

void CCamera::Late_Update()
{
    if (m_bMoveLerp)
    {
        // 목표 위치 계산
        float fTargetX = m_fMoveX;// -m_fMoveX + WINCX * 0.5f;
        float fTargetY = m_fMoveY;// -m_fMoveY + WINCY * 0.5f;

        // 현재 위치 가져오기
        float fCurrentX = Get_X();
        float fCurrentY = Get_Y();

        // 보간 비율 설정 (0.0f ~ 1.0f)
        float fLerpFactor = 0.1f; // 0.1f = 느리게 이동, 1.0f = 즉시 이동

        // LERP 계산
        float fNewX = fCurrentX + (fTargetX - fCurrentX) * fLerpFactor;
        float fNewY = fCurrentY + (fTargetY - fCurrentY) * fLerpFactor;

        // 새로운 위치 설정
        Set_Pos(fNewX, fNewY);

        // 거리 계산 (유클리드 거리)
        float fDeltaX = fTargetX - fCurrentX;
        float fDeltaY = fTargetY - fCurrentY;
        float fDistance = sqrt(fDeltaX * fDeltaX + fDeltaY * fDeltaY);

        // 목표 위치에 가까워지면 리턴
        if (fDistance < 1.0f) { // 1.0f는 임계값(허용 오차)으로 조정 가능
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

    // 객체가 카메라 뷰포트와 겹치는지 확인
    return !(_fX < fLeft || _fX > fRight || _fY < fTop || _fY > fBottom);
}

void CCamera::Render()
{
    // 마우스 위치
    POINT ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(g_hWnd, &ptMouse);

    // 마우스 좌표를 문자열로 변환
    wchar_t szBuf[MAX_STR];
    swprintf(szBuf, 100, L"마우스 스크린 좌표:(%d, %d)", ptMouse.x, ptMouse.y);

    // 폰트로 마우스 좌표 출력
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

    // 카메라 크기에 맞는 사각형 테두리 선 그리기
    DrawCameraBorder();
}

void CCamera::DrawCameraBorder()
{
    // 카메라의 크기와 위치를 가져옵니다.
    float camWidth = m_fViewWidth/2;  // 카메라의 너비
    float camHeight = m_fViewHeight/2; // 카메라의 높이
    float camX = m_tInfo.vPos.x;          // 카메라의 X 좌표
    float camY = m_tInfo.vPos.y;          // 카메라의 Y 좌표

    // 사각형의 네 꼭짓점 좌표 계산
    D3DXVECTOR2 topLeft = D3DXVECTOR2(camX - camWidth, camY - camHeight);
    D3DXVECTOR2 topRight = D3DXVECTOR2(camX + camWidth, camY - camHeight);
    D3DXVECTOR2 bottomLeft = D3DXVECTOR2(camX - camWidth, camY + camHeight);
    D3DXVECTOR2 bottomRight = D3DXVECTOR2(camX + camWidth, camY + camHeight);

    // 선 색상 (흰색)
    D3DCOLOR lineColor = D3DCOLOR_ARGB(255, 255, 255, 255);

    // 선 그리기
    CDevice::Get_Instance()->Get_Line()->SetWidth(2.0f); // 선 두께 설정
    CDevice::Get_Instance()->Get_Line()->Begin();

    // 사각형 테두리 선 그리기
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
    m_fShakeTimer = 0.0f; // 타이머 초기화
}
