#include "pch.h"
#include "Include.h"
#include "CObjMgr.h"
#include "Camera.h"
#include "CDevice.h"

CObjMgr* CObjMgr::m_pInstance = nullptr;

CObjMgr::CObjMgr()
{
}

CObjMgr::~CObjMgr()
{
	Release();
}

void CObjMgr::Add_Object(OBJID eID, CObj* pObj)
{
	if (0 > eID || OBJ_END <= eID || nullptr == pObj)
		return;

	m_ObjList[eID].push_back(pObj);
}

int CObjMgr::Update()
{
	for (size_t i = 0; i < OBJ_END; ++i)
	{
		for (auto iter = m_ObjList[i].begin();
			iter != m_ObjList[i].end(); )
		{
			int iResult = (*iter)->Update();

			if (OBJ_DEAD == iResult)
			{
				Safe_Delete<CObj*>(*iter);
				iter = m_ObjList[i].erase(iter);
			}
			else
				++iter;
		}
	}

	return 0;
}

void CObjMgr::Late_Update()
{
	for (size_t i = 0; i < OBJ_END; ++i)
	{
		for (auto& pObj : m_ObjList[i])
			pObj->Late_Update();
	}
}

void CObjMgr::Render()
{
	for (size_t i = 0; i < OBJ_END; ++i)
	{
		int iIndex = 0;
		TCHAR	szBuf[MIN_STR] = L"";

		for (auto& pObj : m_ObjList[i])
		{

			if (CCamera::Get_Instance()->IsInCameraView(pObj->Get_Info().vPos.x, pObj->Get_Info().vPos.y))
			{
				pObj->Render();

				// 타일 번호 확인용
				/*swprintf_s(szBuf, L"%d", iIndex);
				CDevice::Get_Instance()->Get_Font()->DrawTextW(CDevice::Get_Instance()->Get_Sprite(), szBuf, lstrlen(szBuf), nullptr, 0, D3DCOLOR_ARGB(255, 255, 255, 255));*/

			}
			++iIndex;
		}
	}
}
void CObjMgr::Release()
{
	for (size_t i = 0; i < OBJ_END; ++i)
	{
		for_each(m_ObjList[i].begin(), m_ObjList[i].end(), Safe_Delete<CObj*>);
		m_ObjList[i].clear();
	}


}
