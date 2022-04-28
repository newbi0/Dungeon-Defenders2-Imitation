#include "Timer.h"

_USING(Engine)

CTimer::CTimer()
	: m_fTimeDelta(0.f)
{
}


CTimer::~CTimer()
{
}

HRESULT CTimer::Ready_Timer(void)
{
	QueryPerformanceCounter(&m_FrameTime); // WinApi������ GetTickCount�� ���� ��Ȱ.
	QueryPerformanceCounter(&m_LastTime);
	QueryPerformanceCounter(&m_FixTime);

	QueryPerformanceFrequency(&m_CpuTick); // 1�ʿ� cpu�� ī���� �� �� �ִ� ���ڸ� ����.

	return NOERROR;
}

void CTimer::Compute_TimeDelta(void) // �� �����Ӹ��� ȣ�� ��.
{
	QueryPerformanceCounter(&m_FrameTime);

	m_fTimeDelta = _float(m_FrameTime.QuadPart - m_LastTime.QuadPart) / _float(m_CpuTick.QuadPart);

	m_LastTime = m_FrameTime;

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick); // 1�ʿ� cpu�� ī���� �� �� �ִ� ���ڸ� ����.
		m_FixTime = m_FrameTime;
	}
}

_bool CTimer::Compute_TimeDelta(const _float & fTime)
{
	QueryPerformanceCounter(&m_FrameTime);

	m_fTimeDelta = _float(m_FrameTime.QuadPart - m_LastTime.QuadPart) / _float(m_CpuTick.QuadPart);

	m_LastTime = m_FrameTime;

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart * fTime)
	{
		QueryPerformanceFrequency(&m_CpuTick); // 1�ʿ� cpu�� ī���� �� �� �ִ� ���ڸ� ����.
		m_FixTime = m_FrameTime;
		return true;
	}

	return false;
}

CTimer * CTimer::Create(void)
{
	CTimer*		pInstance = new CTimer;

	// FAILED : ()�ȿ� ����� ���� hr�� 0���� ���� ��쿡 ���ǹ������� ���������� ��ũ�δ�.
	if (FAILED(pInstance->Ready_Timer()))
	{
		_MSG_BOX("CTimer Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTimer::Free(void)
{
}
