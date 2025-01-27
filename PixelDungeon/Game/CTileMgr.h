#pragma once
#include "Define.h"
#include "CTile.h"

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
private:
	void Create_Terrain();
	bool Load_Terrain();
private:
	vector<CObj*> m_vecTile;
};

