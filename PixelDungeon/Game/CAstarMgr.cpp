#include "pch.h"
#include "CAstarMgr.h"
#include "CObjMgr.h"

IMPLEMENT_SINGLETON(CAstarMgr)




CAstarMgr::CAstarMgr()
{
	
}

CAstarMgr::~CAstarMgr()
{
	Release();
}

void CAstarMgr::Convert_Adj()
{
	vector<list<CObj*>>& vecAdjObj = CTileMgr::Get_Instance()->Get_VecAdj();
	m_vecAdj.resize(vecAdjObj.size());

	for (size_t i = 0; i < m_vecAdj.size(); ++i)
	{
		for (CObj* pObj : vecAdjObj[i])
		{
			if (CTile* pTile = dynamic_cast<CTile*>(pObj))
			{
				m_vecAdj[i].push_back(pTile);
			}
		}
	}
	
}

void CAstarMgr::Convert_Tile()
{

	vector<CObj*> vecTileObj = CTileMgr::Get_Instance()->Get_VecTile();
	m_vecTile.resize(vecTileObj.size());


	for (size_t i = 0; i < m_vecTile.size(); ++i)
	{
		m_vecTile[i] = dynamic_cast<CTile*>(vecTileObj[i]);
	}
}

void CAstarMgr::Start_Astar(const D3DXVECTOR3& vStart, const D3DXVECTOR3& vGoal)
{
	Release();
	
	m_iStartIdx = Get_TileIdx(vStart);
	int iGoalIdx = Get_TileIdx(vGoal);

	
	cout << "내가 누른 인덱스 번호 :" << iGoalIdx << endl;
	if (0 > m_iStartIdx ||
		0 > iGoalIdx ||
		(size_t)m_iStartIdx >= m_vecTile.size() ||
		(size_t)iGoalIdx >= m_vecTile.size())
	{
		return;
	}

	// 시작과 도착이 같으면
	if (m_iStartIdx == iGoalIdx)
		return;

	// 장애물 옵션이 있는거 일 경우
	if (m_vecTile[iGoalIdx]->Get_Option() != 0)
		return;

	if (true == Make_Route(m_iStartIdx, iGoalIdx))
	{
		Make_BestList(m_iStartIdx, iGoalIdx);
	}
}




bool CAstarMgr::Make_Route(int iStartIdx, int iGoalIdx)
{ 
	

	// Open 리스트가 비어 있지 않다면 첫 번째 요소 제거
	if (!m_OpenList.empty())
		m_OpenList.pop_front();

	// 현재 노드를 Close 리스트에 추가 (방문 완료)
	m_CloseList.push_back(iStartIdx);

	// 현재 타일의 인접 타일을 확인
	for (auto& pTile : m_vecAdj[iStartIdx])
	{
		// 목표 지점에 도착한 경우 경로 탐색 종료
		if (iGoalIdx == pTile->Get_Tile_Index())
		{
			pTile->Set_PrevTile_Index(iStartIdx);
			return true;
		}

		if (pTile->Get_Option() == OPT_BLOCK)
		{
			continue;
		}
		// Close 리스트와 Open 리스트에 없는 타일만 추가
		if (false == Check_Close(pTile->Get_Tile_Index()) &&
			false == Check_Open(pTile->Get_Tile_Index()))
		{
			pTile->Set_PrevTile_Index(iStartIdx);
			m_OpenList.push_back(pTile->Get_Tile_Index());
		}
	}

	// Open 리스트가 비어 있으면 경로 없음
	if (m_OpenList.empty())
		return false;

	int iStart = m_iStartIdx;

	// Open 리스트를 정렬 (A* 알고리즘의 비용 계산)
	m_OpenList.sort([this, &iGoalIdx, &iStart](int Dst, int Src)->bool
		{
			// 시작 지점에서 해당 타일까지의 비용 (G값)
			D3DXVECTOR3	vPCost1 = m_vecTile[iStart]->Get_Info().vPos - m_vecTile[Dst]->Get_Info().vPos;
			D3DXVECTOR3	vPCost2 = m_vecTile[iStart]->Get_Info().vPos - m_vecTile[Src]->Get_Info().vPos;

			// 해당 타일에서 목표 지점까지의 예상 비용 (H값)
			D3DXVECTOR3	vGCost1 = m_vecTile[iGoalIdx]->Get_Info().vPos - m_vecTile[Dst]->Get_Info().vPos;
			D3DXVECTOR3	vGCost2 = m_vecTile[iGoalIdx]->Get_Info().vPos - m_vecTile[Src]->Get_Info().vPos;

			// F = G + H
			float	fCost1 = D3DXVec3Length(&vPCost1) + D3DXVec3Length(&vGCost1);
			float	fCost2 = D3DXVec3Length(&vPCost2) + D3DXVec3Length(&vGCost2);

			// 비용이 작은 순서대로 정렬
			return fCost1 < fCost2;
		});

	// 가장 비용이 적은 노드부터 다시 탐색
	return Make_Route(m_OpenList.front(), iGoalIdx);
}

void CAstarMgr::Make_BestList(int iStartIdx, int iGoalIdx)
{

	// 목표 지점부터 거슬러 올라가며 최적 경로를 저장
	m_BestList.push_front(m_vecTile[iGoalIdx]);

	int iRouteIdx = m_vecTile[iGoalIdx]->Get_PrevTile_Index();

	while (true)
	{
		// 출발 지점에 도달하면 종료
		if (iRouteIdx == iStartIdx)
			break;

		m_BestList.push_front(m_vecTile[iRouteIdx]);
		iRouteIdx = m_vecTile[iRouteIdx]->Get_PrevTile_Index();
	}

}

int CAstarMgr::Get_TileIdx(const D3DXVECTOR3& vPos)
{
	

	if (m_vecTile.empty())
		return -1;

	// 좌표가 포함된 타일의 인덱스를 찾음
	for (size_t index = 0; index < m_vecTile.size(); ++index)
	{
		if (Picking(vPos, index))
		{
			return index;
		}
	}

	return -1;
}

bool CAstarMgr::Picking(const D3DXVECTOR3& vPos, const int& iIndex)
{
	

	float left = m_vecTile[iIndex]->Get_Info().vPos.x - (TILECX / 2.f);
	float right = m_vecTile[iIndex]->Get_Info().vPos.x + (TILECX / 2.f);
	float top = m_vecTile[iIndex]->Get_Info().vPos.y - (TILECY / 2.f);
	float bottom = m_vecTile[iIndex]->Get_Info().vPos.y + (TILECY / 2.f);

	return (vPos.x >= left && vPos.x <= right &&
		vPos.y >= top && vPos.y <= bottom);
}



bool CAstarMgr::Check_Close(int iIndex)
{
	return find(m_CloseList.begin(), m_CloseList.end(), iIndex) != m_CloseList.end();
}

bool CAstarMgr::Check_Open(int iIndex)
{
	return find(m_OpenList.begin(), m_OpenList.end(), iIndex) != m_OpenList.end();
}


void CAstarMgr::Release()
{
	m_OpenList.clear();
	m_CloseList.clear();
	m_BestList.clear();
	
}


