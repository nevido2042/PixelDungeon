#pragma once

#include "Include.h"

class CTimeMgr
{
	DECLARE_SINGLETON(CTimeMgr)

private:
	CTimeMgr();
	~CTimeMgr();

public:
	float			Get_TimeDelta() {	return m_fTimeDelta;	}

public:
	void			Initialize();
	void			Update();

private:
	LARGE_INTEGER			m_CurTime;
	LARGE_INTEGER			m_OldTime;
	LARGE_INTEGER			m_OriginTime;
	LARGE_INTEGER			m_CpuTick;

	float					m_fTimeDelta;
};

//GetTickCount()										QueryPerformanceCounter()
//
//os ����(������ �����Ǵ� �������� �ð� ���� ����)				�ϵ���� ����(��ǻ�Ͱ� ���õǴ� �������� �ð� ���� ����)
//1 / 1000 ��											1 / 1,000,000 ��
//��� ������												���� �庮�� ����
//														��������� ���ȼ��� �پ