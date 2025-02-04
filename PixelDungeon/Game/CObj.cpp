#include "pch.h"
#include "CObj.h"
#include "Camera.h"

CObj::CObj()
	:m_tTexInfo(nullptr)
{
	ZeroMemory(&m_tInfo, sizeof(INFO));
}

CObj::~CObj()
{
	Release();
}

int CObj::Update()
{
	if (m_tInfo.bDead)
	{
		return OBJ_DEAD;
	}


	UpdateWorldMatrix();

	return OBJ_NOEVENT;
}

void CObj::Release()
{
}

void CObj::UpdateWorldMatrix()
{
	D3DXMATRIX	matWorld, matScale, matTrans;

	float fZoom = CCamera::Get_Instance()->Get_Zoom();

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&matScale, fZoom, fZoom, fZoom);
	D3DXMatrixTranslation(&matTrans,
		m_tInfo.vPos.x,
		m_tInfo.vPos.y,
		m_tInfo.vPos.z);

	m_tInfo.matWorld = matScale * matTrans;
}

