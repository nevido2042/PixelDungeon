#include "pch.h"
#include "CTile.h"
#include "CTextureMgr.h"
#include "CDevice.h"

CTile::CTile()
	:m_byDrawID(0), m_byOption(0)
{
}

CTile::~CTile()
{
	Release();
}

void CTile::Initialize()
{
	m_tTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Terrain", L"Tile", m_byDrawID);
}

void CTile::Late_Update()
{

}

void CTile::Render()
{
	CDevice::Get_Instance()->Get_Sprite()->SetTransform(&m_tInfo.matWorld);

	float	fCenterX = m_tTexInfo->tImgInfo.Width / 2.f;
	float	fCenterY = m_tTexInfo->tImgInfo.Height / 2.f;

	D3DXVECTOR3	vTemp{ fCenterX, fCenterY, 0.f };

	CDevice::Get_Instance()->Get_Sprite()->Draw(m_tTexInfo->pTexture, //����� �ؽ�ó �İ�ü
		nullptr,		// ����� �̹��� ������ ���� Rect �ּ�, null�� ��� �̹����� 0, 0�������� ���
		&vTemp,		// ����� �̹����� �߽� ��ǥ vec3 �ּ�, null�� ��� 0, 0 �̹��� �߽�
		&m_tInfo.vPos,		// ��ġ ��ǥ�� ���� vec3 �ּ�, null�� ��� ��ũ�� �� 0, 0 ��ǥ ���	
		D3DCOLOR_ARGB(255, 255, 255, 255)); // ����� �̹����� ���� ���� ��, 0xffffffff�� �Ѱ��ָ� ���� �ʰ� ���� ���� ����
}
