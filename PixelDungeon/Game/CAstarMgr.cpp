#include "pch.h"
#include "CAstarMgr.h"
#include "CObjMgr.h"
#include "CTileMgr.h"

IMPLEMENT_SINGLETON(CAstarMgr)

CAstarMgr::CAstarMgr()
{
}

CAstarMgr::~CAstarMgr()
{
	Release();
}

void CAstarMgr::Start_Astar(const D3DXVECTOR3& vStart, const D3DXVECTOR3& vGoal)
{
	Release();
	
	//CObj* pMyTile = CObjMgr::Get_Instance()->Get_TILE();
	//vector<CObj*> vecTile = CTileMgr::Get_Instance()->Get_VecTile();
	//dynamic_cast<TILE*>(vecTile.front());

	vector<TILE*> vecTile(reinterpret_cast<vector<TILE*>&>(CTileMgr::Get_Instance()->Get_VecTile())); 

	m_iStartIdx = Get_TileIdx(vStart);
	int iGoalIdx = Get_TileIdx(vGoal);

	
	if (0 > m_iStartIdx ||
		0 > iGoalIdx ||
		(size_t)m_iStartIdx >= vecTile.size() ||
		(size_t)iGoalIdx >= vecTile.size())
	{
		return;
	}

	// ���۰� ������ ������
	if (m_iStartIdx == iGoalIdx)
		return;

	// ��ֹ� �ɼ��� �ִ°� �� ���
	if (0 != vecTile[iGoalIdx]->byOption)
		return;

	if (true == Make_Route(m_iStartIdx, iGoalIdx))
	{
		Make_BestList(m_iStartIdx, iGoalIdx);
	}
}

vector<list<TILE*>> Convert_Adj()
{
	vector<list<CObj*>>& vecAdj = CTileMgr::Get_Instance()->Get_VecAdj();
	vector<list<TILE*>> vecConverted(vecAdj.size());

	for (size_t i = 0; i < vecAdj.size(); ++i)
	{
		for (CObj* pObj : vecAdj[i])
		{
			if (TILE* pTile = dynamic_cast<TILE*>(pObj))  
			{
				vecConverted[i].push_back(pTile);
			}
		}
	}
	return vecConverted;
}


bool CAstarMgr::Make_Route(int iStartIdx, int iGoalIdx)
{ 
	CObj* pMyTerrain = CObjMgr::Get_Instance()->Get_TILE();
	vector<TILE*> vecTile(reinterpret_cast<vector<TILE*>&>(CTileMgr::Get_Instance()->Get_VecTile()));
	vector<list<TILE*>> vecAdj = Convert_Adj();

	// Open ����Ʈ�� ��� ���� �ʴٸ� ù ��° ��� ����
	if (!m_OpenList.empty())
		m_OpenList.pop_front();

	// ���� ��带 Close ����Ʈ�� �߰� (�湮 �Ϸ�)
	m_CloseList.push_back(iStartIdx);

	// ���� Ÿ���� ���� Ÿ���� Ȯ��
	for (auto& pTile : vecAdj[iStartIdx])
	{
		// ��ǥ ������ ������ ��� ��� Ž�� ����
		if (iGoalIdx == pTile->iIndex)
		{
			pTile->iParentIndex = iStartIdx;
			return true;
		}

		// Close ����Ʈ�� Open ����Ʈ�� ���� Ÿ�ϸ� �߰�
		if (false == Check_Close(pTile->iIndex) &&
			false == Check_Open(pTile->iIndex))
		{
			pTile->iParentIndex = iStartIdx;
			m_OpenList.push_back(pTile->iIndex);
		}
	}

	// Open ����Ʈ�� ��� ������ ��� ����
	if (m_OpenList.empty())
		return false;

	int iStart = m_iStartIdx;

	// Open ����Ʈ�� ���� (A* �˰����� ��� ���)
	m_OpenList.sort([&vecTile, &iGoalIdx, &iStart](int Dst, int Src)->bool
		{
			// ���� �������� �ش� Ÿ�ϱ����� ��� (G��)
			D3DXVECTOR3	vPCost1 = vecTile[iStart]->vPos - vecTile[Dst]->vPos;
			D3DXVECTOR3	vPCost2 = vecTile[iStart]->vPos - vecTile[Src]->vPos;

			// �ش� Ÿ�Ͽ��� ��ǥ ���������� ���� ��� (H��)
			D3DXVECTOR3	vGCost1 = vecTile[iGoalIdx]->vPos - vecTile[Dst]->vPos;
			D3DXVECTOR3	vGCost2 = vecTile[iGoalIdx]->vPos - vecTile[Src]->vPos;

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
	vector<TILE*> vecTile(reinterpret_cast<vector<TILE*>&>(CTileMgr::Get_Instance()->Get_VecTile()));

	// ��ǥ �������� �Ž��� �ö󰡸� ���� ��θ� ����
	m_BestList.push_front(vecTile[iGoalIdx]);

	int iRouteIdx = vecTile[iGoalIdx]->iParentIndex;

	while (true)
	{
		// ��� ������ �����ϸ� ����
		if (iRouteIdx == iStartIdx)
			break;

		m_BestList.push_front(vecTile[iRouteIdx]);
		iRouteIdx = vecTile[iRouteIdx]->iParentIndex;
	}

}

int CAstarMgr::Get_TileIdx(const D3DXVECTOR3& vPos)
{
	vector<TILE*> vecTile(reinterpret_cast<vector<TILE*>&>(CTileMgr::Get_Instance()->Get_VecTile()));

	if (vecTile.empty())
		return -1;

	// ��ǥ�� ���Ե� Ÿ���� �ε����� ã��
	for (size_t index = 0; index < vecTile.size(); ++index)
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
	vector<TILE*> vecTile(reinterpret_cast<vector<TILE*>&>(CTileMgr::Get_Instance()->Get_VecTile()));

	D3DXVECTOR3	vPoint[4] =
	{
		{ vecTile[iIndex]->vPos.x, vecTile[iIndex]->vPos.y + (TILECY / 2.f), 0.f },
		{ vecTile[iIndex]->vPos.x + (TILECX / 2.f), vecTile[iIndex]->vPos.y, 0.f },
		{ vecTile[iIndex]->vPos.x, vecTile[iIndex]->vPos.y - (TILECY / 2.f), 0.f },
		{ vecTile[iIndex]->vPos.x - (TILECX / 2.f), vecTile[iIndex]->vPos.y, 0.f },
	};

	for (int i = 0; i < 4; ++i)
	{
		if (0.f < D3DXVec3Dot(&vPoint[i], &vPos))
			return false;
	}
	return true;
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


