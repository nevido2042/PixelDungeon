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

	
	cout << "���� ���� �ε��� ��ȣ :" << iGoalIdx << endl;
	if (0 > m_iStartIdx ||
		0 > iGoalIdx ||
		(size_t)m_iStartIdx >= m_vecTile.size() ||
		(size_t)iGoalIdx >= m_vecTile.size())
	{
		return;
	}

	// ���۰� ������ ������
	if (m_iStartIdx == iGoalIdx)
		return;

	// ��ֹ� �ɼ��� �ִ°� �� ���
	if (m_vecTile[iGoalIdx]->Get_Option() != 0)
		return;

	if (true == Make_Route(m_iStartIdx, iGoalIdx))
	{
		Make_BestList(m_iStartIdx, iGoalIdx);
	}
}




bool CAstarMgr::Make_Route(int iStartIdx, int iGoalIdx)
{ 
	

	// Open ����Ʈ�� ��� ���� �ʴٸ� ù ��° ��� ����
	if (!m_OpenList.empty())
		m_OpenList.pop_front();

	// ���� ��带 Close ����Ʈ�� �߰� (�湮 �Ϸ�)
	m_CloseList.push_back(iStartIdx);

	// ���� Ÿ���� ���� Ÿ���� Ȯ��
	for (auto& pTile : m_vecAdj[iStartIdx])
	{
		// ��ǥ ������ ������ ��� ��� Ž�� ����
		if (iGoalIdx == pTile->Get_Tile_Index())
		{
			pTile->Set_PrevTile_Index(iStartIdx);
			return true;
		}

		if (pTile->Get_Option() == OPT_BLOCK)
		{
			continue;
		}
		// Close ����Ʈ�� Open ����Ʈ�� ���� Ÿ�ϸ� �߰�
		if (false == Check_Close(pTile->Get_Tile_Index()) &&
			false == Check_Open(pTile->Get_Tile_Index()))
		{
			pTile->Set_PrevTile_Index(iStartIdx);
			m_OpenList.push_back(pTile->Get_Tile_Index());
		}
	}

	// Open ����Ʈ�� ��� ������ ��� ����
	if (m_OpenList.empty())
		return false;

	int iStart = m_iStartIdx;

	// Open ����Ʈ�� ���� (A* �˰����� ��� ���)
	m_OpenList.sort([this, &iGoalIdx, &iStart](int Dst, int Src)->bool
		{
			// ���� �������� �ش� Ÿ�ϱ����� ��� (G��)
			D3DXVECTOR3	vPCost1 = m_vecTile[iStart]->Get_Info().vPos - m_vecTile[Dst]->Get_Info().vPos;
			D3DXVECTOR3	vPCost2 = m_vecTile[iStart]->Get_Info().vPos - m_vecTile[Src]->Get_Info().vPos;

			// �ش� Ÿ�Ͽ��� ��ǥ ���������� ���� ��� (H��)
			D3DXVECTOR3	vGCost1 = m_vecTile[iGoalIdx]->Get_Info().vPos - m_vecTile[Dst]->Get_Info().vPos;
			D3DXVECTOR3	vGCost2 = m_vecTile[iGoalIdx]->Get_Info().vPos - m_vecTile[Src]->Get_Info().vPos;

			// F = G + H
			float	fCost1 = D3DXVec3Length(&vPCost1) + D3DXVec3Length(&vGCost1);
			float	fCost2 = D3DXVec3Length(&vPCost2) + D3DXVec3Length(&vGCost2);

			// ����� ���� ������� ����
			return fCost1 < fCost2;
		});

	// ���� ����� ���� ������ �ٽ� Ž��
	return Make_Route(m_OpenList.front(), iGoalIdx);
}

void CAstarMgr::Make_BestList(int iStartIdx, int iGoalIdx)
{

	// ��ǥ �������� �Ž��� �ö󰡸� ���� ��θ� ����
	m_BestList.push_front(m_vecTile[iGoalIdx]);

	int iRouteIdx = m_vecTile[iGoalIdx]->Get_PrevTile_Index();

	while (true)
	{
		// ��� ������ �����ϸ� ����
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

	// ��ǥ�� ���Ե� Ÿ���� �ε����� ã��
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


