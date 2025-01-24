#pragma once

#include "Struct.h"
#include "Define.h"

class CObj abstract
{
public:
	CObj();
	virtual ~CObj();

public:
	INFO		Get_Info() { return m_tInfo; }
	bool		Get_Dead() { return m_tInfo.bDead; }

public:
	virtual void		Initialize()PURE;
	virtual int			Update();
	virtual void		Late_Update()PURE;
	virtual void		Render()PURE;
	virtual void		Release();
protected:
	virtual void		UpdateWorldMatrix();

protected:
	INFO				m_tInfo;
	const TEXINFO*		m_tTexInfo;

};

