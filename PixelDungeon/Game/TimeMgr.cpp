#include "pch.h"
#include "TimeMgr.h"

IMPLEMENT_SINGLETON(CTimeMgr)

CTimeMgr::CTimeMgr()
	: m_fTimeDelta(0.f)
{
	ZeroMemory(&m_OldTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CurTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OriginTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}


CTimeMgr::~CTimeMgr()
{
}

void CTimeMgr::Initialize()
{
	//QueryPerformanceCounter : ���κ��尡 ���� ���ػ� Ÿ�̸��� ���� ���� ������ �Լ�

	QueryPerformanceCounter(&m_CurTime);		// 1000
	QueryPerformanceCounter(&m_OldTime);		// 1024
	
	QueryPerformanceCounter(&m_OriginTime);		// 1030
	// QueryPerformanceFrequency : ���ػ� Ÿ�̸��� ���ļ��� ������ �Լ�, ���⼭ ���ļ��� cpu�� �ʴ� Ŭ������ �ǹ�
	QueryPerformanceFrequency(&m_CpuTick);
}

void CTimeMgr::Update()
{
	QueryPerformanceCounter(&m_CurTime);	// 1100
	
	if (m_CurTime.QuadPart - m_OriginTime.QuadPart > m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_OriginTime = m_CurTime;
	}
	
	m_fTimeDelta = float(m_CurTime.QuadPart - m_OldTime.QuadPart) / m_CpuTick.QuadPart;

	m_OldTime = m_CurTime;
}









