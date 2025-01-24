#pragma once

#include "CScene.h"
#include "Enum.h"
//#include "CLogo.h"
//#include "CMyMenu.h"
//#include "CStage.h"
//#include "CMyEdit.h"

class CSceneMgr
{
private:
	CSceneMgr();
	~CSceneMgr();

public:
	void		Set_Scene(SCENEID eID);
	int			Update();
	void		Late_Update();
	void		Render();
	void		Release();

	SCENEID		Get_SceneID() { return m_eCurScene; }

public:
	static CSceneMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CSceneMgr;

		return m_pInstance;
	}

	static void	Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	static CSceneMgr*	m_pInstance;
	CScene*				m_pScene;

	SCENEID				m_eCurScene;
	SCENEID				m_ePreScene;

};

// 상태 패턴(state) : FSM을 기반으로 하는 객체의 상태를 표현하는 패턴, 자신이 취할 수 있는 유한한 개수의 상태를 가진다는 개념에 근거하여
// 여러 개의 상태 중 반드시 단 하나의 상태만을 취하도록 만드는 성격