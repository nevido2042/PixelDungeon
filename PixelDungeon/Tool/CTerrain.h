#pragma once

#include "Include.h"
#include "ToolView.h"

class CTerrain
{
public:
	CTerrain();
	~CTerrain();

public:
	HRESULT		Initialize();
	void		Update();
	void		Render();
	void		Release();
	void		Mini_Render();

public:
	void	Tile_Change(const D3DXVECTOR3& vPos, const BYTE& byDrawID, const BYTE& byOption);
	void	Set_MainView(CToolView* pMainView) { m_pMainView = pMainView;  }
	void	Set_Ratio(D3DXMATRIX* pOut, float _fX, float _fY);
	void	Save_Tile();

private:
	bool	Picking(const D3DXVECTOR3& vPos, const int& iIndex);
	bool	Picking_Dot(const D3DXVECTOR3& vPos, const int& iIndex);
	int		Get_TileIdx(const D3DXVECTOR3& vPos);
	int		GetPngCount_InDirectory(const CString& directoryPath);
	void	Import_TilePng(); //�Ⱦ�
	void	Create_TileMap();
	bool	Load_Terrain();



private:
	vector<TILE*>		m_vecTile;
	CToolView*			m_pMainView;
};

