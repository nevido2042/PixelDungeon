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

	CDevice::Get_Instance()->Get_Sprite()->Draw(m_tTexInfo->pTexture, //출력할 텍스처 컴객체
		nullptr,		// 출력할 이미지 영역에 대한 Rect 주소, null인 경우 이미지의 0, 0기준으로 출력
		&vTemp,		// 출력할 이미지의 중심 좌표 vec3 주소, null인 경우 0, 0 이미지 중심
		&m_tInfo.vPos,		// 위치 좌표에 대한 vec3 주소, null인 경우 스크린 상 0, 0 좌표 출력	
		D3DCOLOR_ARGB(255, 255, 255, 255)); // 출력할 이미지와 섞을 색상 값, 0xffffffff를 넘겨주면 섞지 않고 원본 색상 유지
}
