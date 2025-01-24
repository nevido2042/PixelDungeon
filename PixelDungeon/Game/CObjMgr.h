#pragma once

#include "Include.h"
#include "CObj.h"

class CObjMgr
{
private:
	CObjMgr();
	CObjMgr(const CObjMgr& rhs) = delete;
	CObj operator =(CObj& rObj) = delete;
	~CObjMgr();

public:
	CObj* Get_Player()		{ return m_ObjList[OBJ_PLAYER].front(); }

public:
	void		Add_Object(OBJID eID, CObj* pObj);
	int			Update();
	void		Late_Update();
	void		Render();
	void		Release();

public:
	static CObjMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CObjMgr;

		return m_pInstance;
	}

	static void		DestroyInstance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	list<CObj*>		m_ObjList[OBJ_END];

	static CObjMgr* m_pInstance;


};

