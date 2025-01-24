#include "pch.h"
#include "CGameTitle.h"
#include "CTextureMgr.h"
#include "CDevice.h"

CGameTitle::CGameTitle()
{
}

CGameTitle::~CGameTitle()
{
	Release();
}

void CGameTitle::Initialize()
{
	m_tTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"GameTitle");

	m_tInfo.vPos.x = WINCX * 0.5f;
	m_tInfo.vPos.y = WINCY * 0.5f;
}

void CGameTitle::Late_Update()
{
}

void CGameTitle::Render()
{
	float	fCenterX = m_tTexInfo->tImgInfo.Width / 2.f;
	float	fCenterY = m_tTexInfo->tImgInfo.Height / 2.f;

	D3DXVECTOR3	vTemp{ fCenterX, fCenterY, 0.f };

	CDevice::Get_Instance()->Get_Sprite()->Draw(m_tTexInfo->pTexture, //����� �ؽ�ó �İ�ü
		nullptr,		// ����� �̹��� ������ ���� Rect �ּ�, null�� ��� �̹����� 0, 0�������� ���
		&vTemp,		// ����� �̹����� �߽� ��ǥ vec3 �ּ�, null�� ��� 0, 0 �̹��� �߽�
		&m_tInfo.vPos,		// ��ġ ��ǥ�� ���� vec3 �ּ�, null�� ��� ��ũ�� �� 0, 0 ��ǥ ���	
		D3DCOLOR_ARGB(255, 255, 255, 255)); // ����� �̹����� ���� ���� ��, 0xffffffff�� �Ѱ��ָ� ���� �ʰ� ���� ���� ����

}

void CGameTitle::Release()
{
}
