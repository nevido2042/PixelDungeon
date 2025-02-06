#include "pch.h"
#include "CGameScene.h"
#include "CObjMgr.h"
#include "CTileMgr.h"
#include "AbstractFactory.h"
#include "CPlayer.h"
#include "Camera.h"


CGameScene::CGameScene()
{
}

CGameScene::~CGameScene()
{
	Release();
}

void CGameScene::Initialize()
{
	CCamera::Get_Instance()->Initialize();
	CCamera::Get_Instance()->Set_Pos(0.f, 0.f);

	CObj* pObj(nullptr);

	pObj = CAbstractFactory<CPlayer>::Create();
	CObjMgr::Get_Instance()->Add_Object(OBJ_PLAYER, pObj);


	CTileMgr::Get_Instance()->Initialize();
}

void CGameScene::Update()
{
	CObjMgr::Get_Instance()->Update();
	CCamera::Get_Instance()->Update();
}

void CGameScene::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();
}

void CGameScene::Render()
{
	CObjMgr::Get_Instance()->Render();
	CCamera::Get_Instance()->Render();
}

void CGameScene::Release()
{
	CObjMgr::DestroyInstance();
}
