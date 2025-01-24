#include "pch.h"
#include "CMainGame.h"
#include "CDevice.h"
#include "AbstractFactory.h"
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

	CSceneMgr::Destroy_Instance();
}

void CMainGame::Load_Texture()
{
	if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(
		L"../Texture/Stage/Player/Stand/AKIHA_AKI00_000.png",
		TEX_SINGLE, L"Player")))
	{
		//AfxMessageBox(L"Terrain Texture Insert Failed");
		return;
	}

	if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(
		L"../Texture/Resources/Logo/banners.png",
		TEX_SINGLE, L"GameTitle")))
	{
		//AfxMessageBox(L"Terrain Texture Insert Failed");
		return;
	}
}
