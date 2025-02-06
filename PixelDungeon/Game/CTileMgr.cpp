#include "pch.h"
#include "CTileMgr.h"
#include "CObjMgr.h"
#include "AbstractFactory.h"
#include "CAstarMgr.h"

IMPLEMENT_SINGLETON(CTileMgr)

CTileMgr::CTileMgr()
{
}

CTileMgr::~CTileMgr()
{
	Release();
}

void CTileMgr::Initialize()
{
	//파일 읽어와서
	//타일 생성
	Create_Terrain();
	
}

void CTileMgr::Update()
{
}

void CTileMgr::Late_Update()
{
}

void CTileMgr::Release()
{
}

void CTileMgr::Ready_Adjacency()
{

	//16x16
    //가로로 30개
	//세로로 20개

	// 좌상단일때 31감소
	// 상하일때 30감소
	m_vecAdj.resize(m_vecTile.size());

	for (int i = 0; i < TILEY; ++i) // Y축으로 행 반복
	{
		for (int j = 0; j < TILEX; ++j) // X축으로 행 반복
		{
			// 현재 타일의 인덱스를 계산
			int iIndex = i * TILEX + j;

			
			// 좌 상단 이동 ( 왼쪽으로 못가고 위로 못갈때 )
			if((0 != i) && (0 != iIndex % (TILEX)))  
			{
			    //  31감소
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex - TILEX-1]);				
			}
			// 위로 이동 
			if ((0 != i) && (0 != iIndex % (TILEX)))
			{
				//  30감소
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex - TILEX]);
			}
			//우 상단 이동
			if ((0 != i) && (TILEX - 1 != iIndex % TILEX))
			{
				// 29 감소
				m_vecAdj[iIndex].push_back(m_vecTile[(iIndex - TILEX)+1]);
			}

			//우로 이동
			if ((TILEX - 1 != iIndex % TILEX))
			{
				// 1증가
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex + 1]); 
			}
			//우하단 이동
			if ((19 != i) && (TILEX - 1 != iIndex % TILEX))
			{
				// 31증가
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex + TILEX + 1]);
			}
			//밑으로 이동
			if (19 != i)
			{
				// 31증가
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex + TILEX ]);
			}
			// 좌 하단 이동
			if ((19 != i) && (0 != iIndex % (TILEX))) 
			{
				 // 29가 증가
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex + TILEX - 1]);
			}

			
			if  ((0 != iIndex % TILEX))
			{
				// 왼쪽으로 1 감소
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex - 1]);
			}

			if (iIndex == 59)
			{
				int  a = 10;
				a += 20;
			}
		}



	}


	
	CAstarMgr::Get_Instance()->Convert_Adj();
	CAstarMgr::Get_Instance()->Convert_Tile();


}

void CTileMgr::Create_Terrain()
{
	if (Load_Terrain())
	{
		/*sort(m_vecTile.begin(), m_vecTile.end(), [&](CObj* Tile_1, CObj* Tile_2)
			{
				if (Tile_1->Get_Info().vPos.y > Tile_2->Get_Info().vPos.y)
				{

				}
			});*/
		Ready_Adjacency();
		return;
	}
	
	//저장 파일이 없으면 아래와 같이
	for (int i = 0; i < TILEY; ++i)
	{
		for (int j = 0; j < TILEX; ++j)
		{
			CObj* pObj = CAbstractFactory<CTile>::Create();
			CObjMgr::Get_Instance()->Add_Object(OBJ_TILE, pObj);
			m_vecTile.push_back(pObj);

			float	fY = float(TILECY * i);
			float	fX = float(TILECX * j);

			pObj->Get_Info().vPos = { fX, fY, 0.f };
			pObj->Get_Info().vSize = { (float)TILECX, (float)TILECY };

			if (CTile* pTile = dynamic_cast<CTile*>(pObj))
			{
				pTile->Set_Option(OPT_PASS);
				pTile->Set_DrawID(0);
				pTile->Set_Tile_Index(i + TILEX + j);
			}
			m_vecTile.push_back(pObj);
		}
	}
	Ready_Adjacency();
}

bool CTileMgr::Load_Terrain()
{
	// 파일 열기 (읽기 모드)
	CStdioFile File;
	if (!File.Open(L"../Save/Terrain/Terrain.txt", CFile::modeRead | CFile::typeBinary))
	{
		AfxMessageBox(_T("저장 파일 없음."));
		return false;
	}

	for (int i = 0; i < TILEY; ++i)
	{
		for (int j = 0; j < TILEX; ++j)
		{
			CObj* pObj = CAbstractFactory<CTile>::Create();
			CObjMgr::Get_Instance()->Add_Object(OBJ_TILE, pObj);
			m_vecTile.push_back(pObj);

			float fY = float(TILECY * i);
			float fX = float(TILECX * j);

			pObj->Get_Info().vPos = { fX, fY, 0.f };
			pObj->Get_Info().vSize = { (float)TILECX, (float)TILECY };

			// 파일에서 byOption과 byDrawID 읽기
			BYTE byOption, byDrawID;

			if (File.Read(&byOption, sizeof(BYTE)) != sizeof(BYTE) ||
				File.Read(&byDrawID, sizeof(BYTE)) != sizeof(BYTE))
			{
				AfxMessageBox(_T("파일 데이터 읽기 오류."));
				delete pObj;
				File.Close();
				return false;
			}

			if (CTile* pTile = dynamic_cast<CTile*>(pObj))
			{
				pTile->Set_Option(byOption);
				pTile->Set_DrawID(byDrawID);
				pTile->Set_Tile_Index(i + TILEX + j);
				pTile->Initialize();
			}
			
			//m_vecTile.push_back(pObj);
		}
	}

	File.Close();

	return true;
}
