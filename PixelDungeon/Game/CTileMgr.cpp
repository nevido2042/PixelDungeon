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
	//���� �о�ͼ�
	//Ÿ�� ����
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
    //���η� 30��
	//���η� 20��

	// �»���϶� 31����
	// �����϶� 30����
	m_vecAdj.resize(m_vecTile.size());

	for (int i = 0; i < TILEY; ++i) // Y������ �� �ݺ�
	{
		for (int j = 0; j < TILEX; ++j) // X������ �� �ݺ�
		{
			// ���� Ÿ���� �ε����� ���
			int iIndex = i * TILEX + j;

			
			// �� ��� �̵� ( �������� ������ ���� ������ )
			if((0 != i) && (0 != iIndex % (TILEX)))  
			{
			    //  31����
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex - TILEX-1]);				
			}
			// ���� �̵� 
			if ((0 != i) && (0 != iIndex % (TILEX)))
			{
				//  30����
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex - TILEX]);
			}
			//�� ��� �̵�
			if ((0 != i) && (TILEX - 1 != iIndex % TILEX))
			{
				// 29 ����
				m_vecAdj[iIndex].push_back(m_vecTile[(iIndex - TILEX)+1]);
			}

			//��� �̵�
			if ((TILEX - 1 != iIndex % TILEX))
			{
				// 1����
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex + 1]); 
			}
			//���ϴ� �̵�
			if ((19 != i) && (TILEX - 1 != iIndex % TILEX))
			{
				// 31����
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex + TILEX + 1]);
			}
			//������ �̵�
			if (19 != i)
			{
				// 31����
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex + TILEX ]);
			}
			// �� �ϴ� �̵�
			if ((19 != i) && (0 != iIndex % (TILEX))) 
			{
				 // 29�� ����
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex + TILEX - 1]);
			}

			
			if  ((0 != iIndex % TILEX))
			{
				// �������� 1 ����
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
	
	//���� ������ ������ �Ʒ��� ����
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
	// ���� ���� (�б� ���)
	CStdioFile File;
	if (!File.Open(L"../Save/Terrain/Terrain.txt", CFile::modeRead | CFile::typeBinary))
	{
		AfxMessageBox(_T("���� ���� ����."));
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

			// ���Ͽ��� byOption�� byDrawID �б�
			BYTE byOption, byDrawID;

			if (File.Read(&byOption, sizeof(BYTE)) != sizeof(BYTE) ||
				File.Read(&byDrawID, sizeof(BYTE)) != sizeof(BYTE))
			{
				AfxMessageBox(_T("���� ������ �б� ����."));
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
