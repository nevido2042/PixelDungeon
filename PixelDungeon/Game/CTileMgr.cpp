#include "pch.h"
#include "CTileMgr.h"
#include "CObjMgr.h"
#include "AbstractFactory.h"

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

	// 타일 인접 관계 설정 시 체크할 조건:
	// 1. 타일이 장애물(옵션 값이 0이 아님)이 아니어야 함.
	// 2. 현재 인덱스가 유효한 범위 내에 있어야 함.

		// 전체 타일을 순회하면서 인접 리스트를 구축함.
	// [ 좌상단 이동 ]
	for (int i = 0; i < TILEY; ++i) // Y축으로 행 반복
	{
		for (int j = 0; j < TILEX; ++i) // X축으로 행 반복
		{
			// 현재 타일의 인덱스를 계산
			int iIndex = i * TILEX + j;

			
			// 첫 번째 행이 아니고, X축의 첫 번째 칸이 아닐 때
			if((0 != i) && (0 != iIndex % (TILEX))) // 좌 상단 이동
			{
			    //  31감소
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex - TILEX-1]);				
			}

			if ((0 != i) && (TILEX - 1 != iIndex % 30)) //우 상단 이동
			{
				// 29 감소
				m_vecAdj[iIndex].push_back(m_vecTile[(iIndex - TILEX)+1]);
			}

			if ((19 != i) && (0 != iIndex % (TILEX))) // 좌 하단 이동
			{
				 // 29가 증가
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex - TILEX - 1]);
			}

			if ((19 != i) && (TILEX - 1 != iIndex % 30)) //우 하단 이동
			{
				// 31 증가
				m_vecAdj[iIndex].push_back(m_vecTile[(iIndex - TILEX) + 1]);
			}
		}



	}


	
	


}

void CTileMgr::Create_Terrain()
{
	if (Load_Terrain())
	{
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
			}
			m_vecTile.push_back(pObj);
		}
	}
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
				pTile->Initialize();
			}
			
			m_vecTile.push_back(pObj);
		}
	}

	File.Close();
	return true;
}
