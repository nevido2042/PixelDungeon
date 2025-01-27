#include "pch.h"
#include "CGameScene.h"
#include "CObjMgr.h"
#include "CTileMgr.h"

CGameScene::CGameScene()
{
}

CGameScene::~CGameScene()
{
	Release();
}

void CGameScene::Initialize()
{
	CTileMgr::Get_Instance()->Initialize();
}

void CGameScene::Update()
{
	CObjMgr::Get_Instance()->Update();
}

void CGameScene::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();
}

void CGameScene::Render()
{
	CObjMgr::Get_Instance()->Render();
}

void CGameScene::Release()
{
	CObjMgr::DestroyInstance();
}
