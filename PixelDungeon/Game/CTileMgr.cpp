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

	// Ÿ�� ���� ���� ���� �� üũ�� ����:
	// 1. Ÿ���� ��ֹ�(�ɼ� ���� 0�� �ƴ�)�� �ƴϾ�� ��.
	// 2. ���� �ε����� ��ȿ�� ���� ���� �־�� ��.

		// ��ü Ÿ���� ��ȸ�ϸ鼭 ���� ����Ʈ�� ������.
	// [ �»�� �̵� ]
	for (int i = 0; i < TILEY; ++i) // Y������ �� �ݺ�
	{
		for (int j = 0; j < TILEX; ++i) // X������ �� �ݺ�
		{
			// ���� Ÿ���� �ε����� ���
			int iIndex = i * TILEX + j;

			
			// ù ��° ���� �ƴϰ�, X���� ù ��° ĭ�� �ƴ� ��
			if((0 != i) && (0 != iIndex % (TILEX))) // �� ��� �̵�
			{
			    //  31����
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex - TILEX-1]);				
			}

			if ((0 != i) && (TILEX - 1 != iIndex % 30)) //�� ��� �̵�
			{
				// 29 ����
				m_vecAdj[iIndex].push_back(m_vecTile[(iIndex - TILEX)+1]);
			}

			if ((19 != i) && (0 != iIndex % (TILEX))) // �� �ϴ� �̵�
			{
				 // 29�� ����
				m_vecAdj[iIndex].push_back(m_vecTile[iIndex - TILEX - 1]);
			}

			if ((19 != i) && (TILEX - 1 != iIndex % 30)) //�� �ϴ� �̵�
			{
				// 31 ����
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
			}
			m_vecTile.push_back(pObj);
		}
	}
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
				pTile->Initialize();
			}
			
			m_vecTile.push_back(pObj);
		}
	}

	File.Close();
	return true;
}
