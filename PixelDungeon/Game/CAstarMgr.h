#pragma once

#include "include.h" 


#include "CTileMgr.h"
#include "CTile.h"


class CAstarMgr
{
public:
	DECLARE_SINGLETON(CAstarMgr)

private:
	CAstarMgr();
	~CAstarMgr();
public:
	list<CTile*>& Get_BestList() { return m_BestList; } 

public:
	//나 에이스타 시작한다
	void Start_Astar(const D3DXVECTOR3& vStart, const D3DXVECTOR3& vGoal);
	void Release();
	void Convert_Tile();
	void Convert_Adj(); 

private:
	bool Make_Route(int iStartIdx, int iGoalIdx);
	void Make_BestList(int iStartIdx, int iGoalIdx);

	int Get_TileIdx(const D3DXVECTOR3& vPos);
	bool Picking(const D3DXVECTOR3& vPos, const int& iIndex);
	

private:
	bool Check_Close(int iIndex);
	bool Check_Open(int iIndex);


private:
	list<int> m_OpenList;
	list<int> m_CloseList;

	list<CTile*> m_BestList;

	int m_iStartIdx;


	vector<CTile*> m_vecTile;
	vector<list<CTile*>> m_vecAdj;

};

