#include "pch.h"
#include "Camera.h"
#include "CObjMgr.h"
#include "CKeyMgr.h"

CCamera* CCamera::m_pInstance = nullptr;

CCamera::CCamera()
    :m_fHeight(0.f), m_fWidth(0.f), m_fX(0.f), m_fY(0.f), m_fZoom(0.f),
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

    m_fX = m_fWidth * 0.5f;
    m_fY = m_fHeight * 0.5f;

    m_fZoom = 1.f;
}

void CCamera::Update()
{
    float fSpeed(10.f);
    if (CKeyMgr::Get_Instance()->Key_Pressing('A'))
    {
        m_bMoveLerp = false;
        m_fX -= fSpeed;
    }
    if (CKeyMgr::Get_Instance()->Key_Pressing('D'))
    {
        m_bMoveLerp = false;
        m_fX += fSpeed;
    }
    if (CKeyMgr::Get_Instance()->Key_Pressing('W'))
    {
        m_bMoveLerp = false;
        m_fY -= fSpeed;
    }
    if (CKeyMgr::Get_Instance()->Key_Pressing('S'))
    {
        m_bMoveLerp = false;
        m_fY += fSpeed;
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
        float finalX = m_fX;
        float finalY = m_fY;

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
        m_fX = finalX;
        m_fY = finalY;
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
    float fLeft = m_fX - (m_fWidth * 0.5f / m_fZoom);
    float fRight = m_fX + (m_fWidth * 0.5f / m_fZoom);
    float fTop = m_fY - (m_fHeight * 0.5f / m_fZoom);
    float fBottom = m_fY + (m_fHeight * 0.5f / m_fZoom);

    m_fViewWidth = abs(fLeft - fRight);
    m_fViewHeight = abs(fTop - fBottom);

    // 객체가 카메라 뷰포트와 겹치는지 확인
    return !(_fX < fLeft || _fX > fRight || _fY < fTop || _fY > fBottom);
}

void CCamera::Render(HDC hDC)
{
    // 변수 값을 유니코드 문자열로 변환
    //wchar_t buffer[50];
    //wsprintf(buffer, L"m_iCurFrame: %d", m_iCurFrame);
    //// 문자열 출력 (유니코드)
    //TextOutW(hDC, 0, 0, buffer, lstrlenW(buffer));

    //마우스 위치
    POINT	ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(g_hWnd, &ptMouse);

    wchar_t buffer[256];
    swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"Mouse Screen: %.2f, %.2f", (double)ptMouse.x, (double)ptMouse.y);
    // 문자열 출력 (유니코드)
    TextOutW(hDC, 0, 20, buffer, lstrlenW(buffer));

    POINT tWorldPoint = ScreenToWorld((int)ptMouse.x, (int)ptMouse.y);

    swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"Mouse World: %.2f, %.2f", (double)tWorldPoint.x, (double)tWorldPoint.y);
    // 문자열 출력 (유니코드)
    TextOutW(hDC, 0, 40, buffer, lstrlenW(buffer));

    swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"Camera W/H: %.2f, %.2f", (double)m_fWidth, (double)m_fHeight);
    // 문자열 출력 (유니코드)
    TextOutW(hDC, 0, 60, buffer, lstrlenW(buffer));

    swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"Camera X/Y: %.2f, %.2f", (double)m_fX, (double)m_fY);
    // 문자열 출력 (유니코드)
    TextOutW(hDC, 0, 80, buffer, lstrlenW(buffer));

    swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"Camera Zoom: %.2f", (double)m_fZoom);
    // 문자열 출력 (유니코드)
    TextOutW(hDC, 0, 100, buffer, lstrlenW(buffer));

    swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"View W/H: %.2f, %.2f", (double)m_fViewWidth, (double)m_fViewHeight);
    // 문자열 출력 (유니코드)
    TextOutW(hDC, 0, 120, buffer, lstrlenW(buffer));
}

void CCamera::Start_Shake(float intensity, float duration)
{
    m_bIsShaking = true;
    m_fShakeIntensity = intensity;
    m_fShakeDuration = duration;
    m_fShakeTimer = 0.0f; // 타이머 초기화
}
