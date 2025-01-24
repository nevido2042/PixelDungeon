#include "pch.h"
#include "CPlayer.h"
#include "CDevice.h"
#include "CTextureMgr.h"

CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
	Release();
}

void CPlayer::Initialize()
{
	m_tTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Player");
}

int CPlayer::Update()
{
	m_tInfo.vPos.x += 1.f;
	m_tInfo.vPos.y += 1.f;

	return CObj::Update();
}

void CPlayer::Late_Update()
{
}

void CPlayer::Render()
{
		TCHAR	szBuf[MIN_STR] = L"";

		//const TEXINFO* pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Player");

		float	fCenterX = m_tTexInfo->tImgInfo.Width / 2.f;
		float	fCenterY = m_tTexInfo->tImgInfo.Height / 2.f;

		D3DXVECTOR3	vTemp{ fCenterX, fCenterY, 0.f };

		CDevice::Get_Instance()->Get_Sprite()->Draw(m_tTexInfo->pTexture, //출력할 텍스처 컴객체
			nullptr,		// 출력할 이미지 영역에 대한 Rect 주소, null인 경우 이미지의 0, 0기준으로 출력
			&vTemp,		// 출력할 이미지의 중심 좌표 vec3 주소, null인 경우 0, 0 이미지 중심
			&m_tInfo.vPos,		// 위치 좌표에 대한 vec3 주소, null인 경우 스크린 상 0, 0 좌표 출력	
			D3DCOLOR_ARGB(255, 255, 255, 255)); // 출력할 이미지와 섞을 색상 값, 0xffffffff를 넘겨주면 섞지 않고 원본 색상 유지

		swprintf_s(szBuf, L"Player");
		RECT tRect{ (long)m_tInfo.vPos.x, (long)m_tInfo.vPos.y, (long)m_tInfo.vPos.x + 100, (long)m_tInfo.vPos.x + 100 };
		CDevice::Get_Instance()->Get_Font()->DrawTextW(CDevice::Get_Instance()->Get_Sprite(),
			szBuf,		// 출력할 문자열
			lstrlen(szBuf),  // 문자열 버퍼의 크기
			&tRect,	// 출력할 렉트 위치
			0,			// 정렬 기준(옵션)
			D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CPlayer::Release()
{
}


