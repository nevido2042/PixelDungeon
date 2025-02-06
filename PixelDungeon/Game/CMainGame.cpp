#include "pch.h"
#include "CMainGame.h"
#include "CDevice.h"
#include "AbstractFactory.h"
#include "CSceneMgr.h"
#include "TimeMgr.h"
#include "CAstarMgr.h"
#include "Camera.h"
#include "CTileMgr.h"
#include "CKeyMgr.h"
#include "CSceneMgr.h"

CMainGame::CMainGame()
{
}

CMainGame::~CMainGame()
{
	Release();
}

void CMainGame::Initialize()
{
	if (FAILED(CDevice::Get_Instance()->Init_Device()))
	{
		//AfxMessageBox(L"Init_Device Create Failed");
		return;
	}

	Load_Texture();

	CSceneMgr::Get_Instance()->Set_Scene(SC_MENU);

#ifdef _DEBUG

	if (::AllocConsole() == TRUE)
	{
		FILE* nfp[3];
		freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
		std::ios::sync_with_stdio();
	}

#endif // _DEBUG
}

void CMainGame::Update()
{
	CSceneMgr::Get_Instance()->Update();
	CTimeMgr::Get_Instance()->Update();
}

void CMainGame::Late_Update()
{
	CSceneMgr::Get_Instance()->Late_Update();
}

void CMainGame::Render()
{
	CDevice::Get_Instance()->Render_Begin();

	CSceneMgr::Get_Instance()->Render();

	CDevice::Get_Instance()->Render_End();
}

void CMainGame::Release()
{
#ifdef _DEBUG

	FreeConsole();

#endif // _DEBUG

	CAstarMgr::Destroy_Instance();
	CTimeMgr::Destroy_Instance();
	CTextureMgr::Get_Instance()->Destroy_Instance();
	CSceneMgr::Get_Instance()->Destroy_Instance();
	CCamera::Get_Instance()->Destroy_Instance();
	CTileMgr::Get_Instance()->Destroy_Instance();
	CKeyMgr::Get_Instance()->Destroy_Instance();
	CSceneMgr::Get_Instance()->Destroy_Instance();
	CDevice::Get_Instance()->Destroy_Instance();
	
}

void CMainGame::Load_Texture()
{
	//if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(
	//	L"../Resources/Player/Warrior/warrior1.png",
	//	TEX_SINGLE, L"Player")))
	//{
	//	//AfxMessageBox(L"Terrain Texture Insert Failed");
	//	return;
	//}

	if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(
		L"../Resources/Logo/banners.png",
		TEX_SINGLE, L"GameTitle")))
	{
		//AfxMessageBox(L"Terrain Texture Insert Failed");
		return;
	}

	CString folderPath = L"../Texture/Terrain/Tile"; // 경로 설정
	int fileCount = GetPngCount_InDirectory(folderPath);

	if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(
		L"../Texture/Terrain/Tile/tile_sewers%03d.png",
		TEX_MULTI, L"Terrain", L"Tile", fileCount)))
	{
		AfxMessageBox(L"Terrain Texture Insert Failed");
	}
}

int CMainGame::GetPngCount_InDirectory(const CString& directoryPath)
{
	int fileCount = 0;

	WIN32_FIND_DATA findData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// 폴더 경로에 *.* 필터를 추가하여 모든 파일을 찾도록 설정
	CString searchPath = directoryPath + _T("\\*.*");

	hFind = FindFirstFile(searchPath, &findData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		// 폴더를 찾을 수 없는 경우
		AfxMessageBox(_T("폴더를 찾을 수 없습니다."), MB_OK | MB_ICONERROR);
		return 0;
	}

	do
	{
		// 디렉토리는 제외하고, 파일만 .png 확장자 확인
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			CString fileName = findData.cFileName;
			int dotPos = fileName.ReverseFind(_T('.'));

			// 확장자가 .png인 파일만 카운트
			if (dotPos != -1 && fileName.Mid(dotPos + 1).CompareNoCase(_T("png")) == 0)
			{
				fileCount++;
			}
		}
	} while (FindNextFile(hFind, &findData) != 0);

	FindClose(hFind); // 핸들 종료
	return fileCount;
}
