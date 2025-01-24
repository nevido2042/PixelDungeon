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

// ���� ����(state) : FSM�� ������� �ϴ� ��ü�� ���¸� ǥ���ϴ� ����, �ڽ��� ���� �� �ִ� ������ ������ ���¸� �����ٴ� ���信 �ٰ��Ͽ�
// ���� ���� ���� �� �ݵ�� �� �ϳ��� ���¸��� ���ϵ��� ����� ����