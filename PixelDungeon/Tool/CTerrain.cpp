#include "pch.h"
#include "CTerrain.h"
#include "CTextureMgr.h"
#include "CDevice.h"

CTerrain::CTerrain()
	:m_pMainView(nullptr)
{
	m_vecTile.reserve(TILEX * TILEY);
}

CTerrain::~CTerrain()
{
	Release();
}

HRESULT CTerrain::Initialize()
{
	Import_TilePng();
	Create_TileMap();

	return S_OK;
}

void CTerrain::Update()
{
	
}

void CTerrain::Render()
{
	D3DXMATRIX	matWorld, matScale, matTrans;

	TCHAR	szBuf[MIN_STR] = L"";
	int		iIndex(0);

	for (auto pTile : m_vecTile)
	{
		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
		D3DXMatrixTranslation(&matTrans,
			pTile->vPos.x - m_pMainView->GetScrollPos(0),
			pTile->vPos.y - m_pMainView->GetScrollPos(1),
			pTile->vPos.z);

		matWorld = matScale * matTrans;

		RECT	rc{};
		GetClientRect(m_pMainView->m_hWnd, &rc);

		float	fX = WINCX / float(rc.right - rc.left);
		float	fY = WINCY / float(rc.bottom - rc.top);

		float fZoom = m_pMainView->Get_Zoom();

		Set_Ratio(&matWorld, fX * fZoom, fY * fZoom);

		CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matWorld);

		const TEXINFO* pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Terrain", L"Tile", pTile->byDrawID);

		float	fCenterX = pTexInfo->tImgInfo.Width / 2.f;
		float	fCenterY = pTexInfo->tImgInfo.Height / 2.f;

		D3DXVECTOR3	vTemp{ fCenterX, fCenterY, 0.f };

		// ��������Ʈ�� Ÿ�� �ؽ�ó ������
		CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture,
			nullptr, &vTemp, nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));

		iIndex++;
	}

	// ��������Ʈ ���� �� �� �׸���
	CDevice::Get_Instance()->Get_Sprite()->End();

	for (auto pTile : m_vecTile)
	{
		if (pTile->byOption == 1) // Ư�� ���ǿ��� X ǥ��
		{
			ID3DXLine* pLine;
			if (SUCCEEDED(D3DXCreateLine(CDevice::Get_Instance()->Get_Device(), &pLine)))
			{
				D3DXVECTOR2 vLines[4] =
				{
					{ pTile->vPos.x - TILECX * 0.5f - m_pMainView->GetScrollPos(0), pTile->vPos.y - TILECY * 0.5f -m_pMainView->GetScrollPos(1) },
					{ pTile->vPos.x + TILECX * 0.5f - m_pMainView->GetScrollPos(0), pTile->vPos.y + TILECY * 0.5f -m_pMainView->GetScrollPos(1) },
					{ pTile->vPos.x + TILECX * 0.5f - m_pMainView->GetScrollPos(0), pTile->vPos.y - TILECY * 0.5f -m_pMainView->GetScrollPos(1) },
					{ pTile->vPos.x - TILECX * 0.5f - m_pMainView->GetScrollPos(0), pTile->vPos.y + TILECY * 0.5f -m_pMainView->GetScrollPos(1) }
				};


				pLine->SetWidth(2.0f); // �� �β�
				pLine->Begin();
				pLine->Draw(vLines, 2, D3DCOLOR_ARGB(255, 255, 0, 0)); // ù ��° �밢�� (������)
				pLine->Draw(&vLines[2], 2, D3DCOLOR_ARGB(255, 255, 0, 0)); // �� ��° �밢�� (������)
				pLine->End();
				pLine->Release();
			}
		}
	}

	// ��������Ʈ �ٽ� ����
	CDevice::Get_Instance()->Get_Sprite()->Begin(D3DXSPRITE_ALPHABLEND);
}

void CTerrain::Release()
{
	for_each(m_vecTile.begin(), m_vecTile.end(), [](auto& p) 
		{ 
			if (p)
			{ delete p; p = nullptr; }
		});
	m_vecTile.clear();
	m_vecTile.shrink_to_fit();
}

void CTerrain::Mini_Render()
{
	D3DXMATRIX	matWorld, matScale, matTrans;

	for (auto pTile : m_vecTile)
	{
		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
		D3DXMatrixTranslation(&matTrans,
			pTile->vPos.x,
			pTile->vPos.y,
			pTile->vPos.z);

		matWorld = matScale * matTrans;
				
		Set_Ratio(&matWorld, 0.3f, 0.3f);

		CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matWorld);

		const TEXINFO* pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Terrain", L"Tile", pTile->byDrawID);

		float	fCenterX = pTexInfo->tImgInfo.Width / 2.f;
		float	fCenterY = pTexInfo->tImgInfo.Height / 2.f;

		D3DXVECTOR3	vTemp{ fCenterX, fCenterY, 0.f };

		CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture, //����� �ؽ�ó �İ�ü
			nullptr,		// ����� �̹��� ������ ���� Rect �ּ�, null�� ��� �̹����� 0, 0�������� ���
			&vTemp,		// ����� �̹����� �߽� ��ǥ vec3 �ּ�, null�� ��� 0, 0 �̹��� �߽�
			nullptr,		// ��ġ ��ǥ�� ���� vec3 �ּ�, null�� ��� ��ũ�� �� 0, 0 ��ǥ ���	
			D3DCOLOR_ARGB(255, 255, 255, 255)); // ����� �̹����� ���� ���� ��, 0xffffffff�� �Ѱ��ָ� ���� �ʰ� ���� ���� ����
	}
}

void CTerrain::Tile_Change(const D3DXVECTOR3& vPos, const BYTE& byDrawID)
{
	int iIndex = Get_TileIdx(vPos);

	if (-1 == iIndex)
		return;

	//��Ƽ�ؽ��Ŀ��� byDrawID�� ���ڸ� ���� �ָ� ã�Ƽ� 
	//int iTexIndex = CTextureMgr::Get_Instance()->Find_MultiTex_Index(L"Terrain", byDrawID);
	//�� �ε����� �־��


	m_vecTile[iIndex]->byDrawID = byDrawID;
	m_vecTile[iIndex]->byOption = 1;
}

void CTerrain::Set_Ratio(D3DXMATRIX* pOut, float _fX, float _fY)
{
	pOut->_11 *= _fX;
	pOut->_21 *= _fX;
	pOut->_31 *= _fX;
	pOut->_41 *= _fX;

	pOut->_12 *= _fY;
	pOut->_22 *= _fY;
	pOut->_32 *= _fY;
	pOut->_42 *= _fY;

}


int CTerrain::Get_TileIdx(const D3DXVECTOR3& vPos)
{
	for (size_t index = 0; index < m_vecTile.size(); ++index)
	{
		if (Picking_Dot(vPos, (int)index))
		{
			return (int)index;
		}
	}

	return -1;
}

int CTerrain::GetPngCount_InDirectory(const CString& directoryPath)
{
	int fileCount = 0;

	WIN32_FIND_DATA findData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// ���� ��ο� *.* ���͸� �߰��Ͽ� ��� ������ ã���� ����
	CString searchPath = directoryPath + _T("\\*.*");

	hFind = FindFirstFile(searchPath, &findData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		// ������ ã�� �� ���� ���
		AfxMessageBox(_T("������ ã�� �� �����ϴ�."), MB_OK | MB_ICONERROR);
		return 0;
	}

	do
	{
		// ���丮�� �����ϰ�, ���ϸ� .png Ȯ���� Ȯ��
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			CString fileName = findData.cFileName;
			int dotPos = fileName.ReverseFind(_T('.'));

			// Ȯ���ڰ� .png�� ���ϸ� ī��Ʈ
			if (dotPos != -1 && fileName.Mid(dotPos + 1).CompareNoCase(_T("png")) == 0)
			{
				fileCount++;
			}
		}
	} while (FindNextFile(hFind, &findData) != 0);

	FindClose(hFind); // �ڵ� ����
	return fileCount;
}

void CTerrain::Import_TilePng()
{
	CString folderPath = L"../Resources/Tile/tiles_sewers"; // ��� ����
	int fileCount = GetPngCount_InDirectory(folderPath);

	if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(
		L"../Resources/Tile/tiles_sewers/%02d_tiles_sewers.png",
		TEX_MULTI, L"Terrain", L"Tile", fileCount)))
	{
		AfxMessageBox(L"Terrain Texture Insert Failed");
	}

	/*CString folderPath = L"../Resources/Tile/tiles_sewers";
	CFileFind finder;
	BOOL bWorking = finder.FindFile(folderPath + L"/*.png");
	std::vector<CString> filePaths;

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (!finder.IsDots() && !finder.IsDirectory())
		{
			filePaths.push_back(finder.GetFilePath());
		}
	}
	finder.Close();

	if (filePaths.empty())
	{
		AfxMessageBox(L"No PNG files found in the directory.");
		return;
	}

	if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(filePaths, TEX_MULTI, L"Terrain", L"Tile")))
	{
		AfxMessageBox(L"Failed to insert textures for Terrain.");
	}*/
}

void CTerrain::Create_TileMap()
{
	//���� ������ �ִ� �� Ȯ��
	//���� ������ ������ �������ϴ�� drawID�� option ����
	if (!Load_Terrain())
	{
		//���� ������ ������ �Ʒ��� ����
		for (int i = 0; i < TILEY; ++i)
		{
			for (int j = 0; j < TILEX; ++j)
			{
				TILE* pTile = new TILE;

				float	fY = float(TILECY * i);// (TILECY / 2.f)* i;
				float	fX = float(TILECX * j);// (TILECX * j) + (i % 2) * (TILECX / 2.f);

				pTile->vPos = { fX, fY, 0.f };
				pTile->vSize = { (float)TILECX, (float)TILECY };
				pTile->byOption = 0;
				pTile->byDrawID = 0;

				m_vecTile.push_back(pTile);
			}
		}
	}

	

}

bool CTerrain::Load_Terrain()
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
			TILE* pTile = new TILE;

			float fY = float(TILECY * i);
			float fX = float(TILECX * j);

			pTile->vPos = { fX, fY, 0.f };
			pTile->vSize = { (float)TILECX, (float)TILECY };

			// ���Ͽ��� byOption�� byDrawID �б�
			BYTE byOption, byDrawID;
			if (File.Read(&byOption, sizeof(BYTE)) != sizeof(BYTE) ||
				File.Read(&byDrawID, sizeof(BYTE)) != sizeof(BYTE))
			{
				AfxMessageBox(_T("���� ������ �б� ����."));
				delete pTile;
				File.Close();
				return false;
			}

			pTile->byOption = byOption;
			pTile->byDrawID = byDrawID;

			m_vecTile.push_back(pTile);
		}
	}

	File.Close();
	return true;
}

void CTerrain::Save_Tile()
{
	// ���� ���� (���� ���, �� ���� ����)
	CStdioFile File;
	if (!File.Open(L"../Save/Terrain/Terrain.txt", CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		AfxMessageBox(_T("���� ���� ���� �Ұ�."));
	}

	// ���� �����͸� ��ȸ�ϸ� byOption�� byDrawID�� ���Ͽ� ����
	for (size_t i = 0; i < m_vecTile.size(); ++i)
	{
		if (m_vecTile[i] != nullptr)
		{
			// byOption�� byDrawID�� ���� ����
			BYTE byOption = m_vecTile[i]->byOption;
			BYTE byDrawID = m_vecTile[i]->byDrawID;

			File.Write(&byOption, sizeof(BYTE)); // byOption ����
			File.Write(&byDrawID, sizeof(BYTE)); // byDrawID ����
		}
	}

	// ���� �ݱ�
	File.Close();
}

bool CTerrain::Picking(const D3DXVECTOR3& vPos, const int& iIndex)
{
	// 12 -> 3 -> 6 -> 9
	float	fGradient[4] =
	{
		(TILECY / 2.f) / (TILECX / 2.f) * -1.f,
		(TILECY / 2.f) / (TILECX / 2.f),
		(TILECY / 2.f) / (TILECX / 2.f) * -1.f,
		(TILECY / 2.f) / (TILECX / 2.f)
	};

	// 12 -> 3 -> 6 -> 9
	D3DXVECTOR3	vPoint[4] =
	{
		{ m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y + (TILECY / 2.f), 0.f },
		{ m_vecTile[iIndex]->vPos.x + (TILECX / 2.f), m_vecTile[iIndex]->vPos.y, 0.f },
		{ m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y - (TILECY / 2.f), 0.f },
		{ m_vecTile[iIndex]->vPos.x - (TILECX / 2.f), m_vecTile[iIndex]->vPos.y, 0.f },

	};

	// y = ax + b;
	// -b = ax - y
	// b = y - ax

	float	fB[4] =
	{
		vPoint[0].y - fGradient[0] * vPoint[0].x,
		vPoint[1].y - fGradient[1] * vPoint[1].x,
		vPoint[2].y - fGradient[2] * vPoint[2].x,
		vPoint[3].y - fGradient[3] * vPoint[3].x
	};

	// 0 == ax + b - y // �� �� ����
	// 0 > ax + b - y // ���� ����
	// 0 < ax + b - y // ���� �Ʒ���

	bool	bCheck[4]{ false };

	// 12 -> 3
	if (0 < fGradient[0] * vPos.x + fB[0] - vPos.y)
		bCheck[0] = true;

	// 3 -> 6
	if (0 >= fGradient[1] * vPos.x + fB[1] - vPos.y)
		bCheck[1] = true;

	// 6 -> 9
	if (0 >= fGradient[2] * vPos.x + fB[2] - vPos.y)
		bCheck[2] = true;

	// 9 -> 12
	if (0 < fGradient[3] * vPos.x + fB[3] - vPos.y)
		bCheck[3] = true;


	return bCheck[0] && bCheck[1] && bCheck[2] && bCheck[3];
}

bool CTerrain::Picking_Dot(const D3DXVECTOR3& vPos, const int& iIndex)
{
	// 12 -> 3 -> 6 -> 9
	D3DXVECTOR3	vPoint[4] =
	{
		{ m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y + (TILECY / 2.f), 0.f },
		{ m_vecTile[iIndex]->vPos.x + (TILECX / 2.f), m_vecTile[iIndex]->vPos.y, 0.f },
		{ m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y - (TILECY / 2.f), 0.f },
		{ m_vecTile[iIndex]->vPos.x - (TILECX / 2.f), m_vecTile[iIndex]->vPos.y, 0.f },

	};

	D3DXVECTOR3	vDir[4] =
	{
		vPoint[1] - vPoint[0],
		vPoint[2] - vPoint[1],
		vPoint[3] - vPoint[2],
		vPoint[0] - vPoint[3]
	};

	D3DXVECTOR3		vNormal[4] =
	{
		{ -vDir[0].y , vDir[0].x, 0.f },
		{ -vDir[1].y , vDir[1].x, 0.f },
		{ -vDir[2].y , vDir[2].x, 0.f },
		{ -vDir[3].y , vDir[3].x, 0.f },
	};

	D3DXVECTOR3		vMouseDir[4] =
	{
		vPos - vPoint[0],
		vPos - vPoint[1],
		vPos - vPoint[2],
		vPos - vPoint[3]
	};

	for (int i = 0; i < 4; ++i)
	{
		D3DXVec3Normalize(&vNormal[i], &vNormal[i]);
		D3DXVec3Normalize(&vMouseDir[i], &vMouseDir[i]);
	}

	for (int i = 0; i < 4; ++i)
	{
		if (0.f < D3DXVec3Dot(&vNormal[i], &vMouseDir[i]))
			return false;
	}

	return true;
}
