#pragma once

#include "Include.h"

#define ZOOM_MAX 6.f
#define ZOOM_MIN 0.2f

class CCamera
{
private:
	CCamera();
	~CCamera();
public:
	POINT WorldToScreen(float worldX, float worldY);
	D3DXMATRIX WorldToScreen(const D3DXMATRIX& matWorld);
	POINT ScreenToWorld(int screenX, int screenY);
	D3DXVECTOR3 ScreenToWorld(const D3DXVECTOR3& vecScreen);

	void Move_To_Lerp(float _fX, float _fY);
public:
	void Initialize();
	void Update();
	void Late_Update();
	void Render();
public:
	void Start_Shake(float intensity, float duration);
	//void ApplyPosition(float x, float y);
public:
	float	Get_X() { return m_tInfo.vPos.x; }
	float	Get_Y() { return m_tInfo.vPos.y; }
	void	Set_Pos(float _fX, float _fY) { m_tInfo.vPos.x = _fX, m_tInfo.vPos.y = _fY; }
	float	Get_Zoom() { return m_fZoom; }
	void	Set_Zoom(float _fZoom) { m_fZoom = _fZoom; }
	bool	IsInCameraView(float _fX, float _fY);
public:
	static CCamera* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CCamera;

		return m_pInstance;
	}

	static void	Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	void DrawCameraBorder();

private:
	static CCamera* m_pInstance;

	INFO	m_tInfo;

	//float	m_fX;
	//float	m_fY;

	float	m_fWidth;
	float	m_fHeight;
	float	m_fZoom; // 줌 비율 (1.0 = 기본 크기)

	float	m_fViewWidth;
	float	m_fViewHeight;

	//보간이동
	float	m_fMoveX;
	float	m_fMoveY;
	bool	m_bMoveLerp;

	// 카메라 쉐이킹 관련 변수
	bool m_bIsShaking = false; // 쉐이킹 활성화 여부
	float m_fShakeDuration = 0.0f; // 쉐이킹 지속 시간
	float m_fShakeIntensity = 0.0f; // 쉐이킹 강도
	float m_fShakeTimer = 0.0f; // 쉐이킹 시간 누적
};
