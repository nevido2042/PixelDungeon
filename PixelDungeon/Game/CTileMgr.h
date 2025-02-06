#pragma once
#include "Define.h"
#include "CTile.h"
#include "Include.h"

class CTileMgr
{
	DECLARE_SINGLETON(CTileMgr)

private:
	CTileMgr();
	~CTileMgr();
public:
	void Initialize();
	void Update();
	void Late_Update();
	void Release();

	vector<CObj*>& Get_VecTile() { return m_vecTile; }
	vector<list<CObj*>>& Get_VecAdj() { return m_vecAdj; }
	void Ready_Adjacency();
private:
	
	void Create_Terrain();
	bool Load_Terrain();
private:
	vector<CObj*> m_vecTile;
	vector<list<CObj*>> m_vecAdj;
};

