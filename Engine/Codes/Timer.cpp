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
	QueryPerformanceCounter(&m_FrameTime); // WinApi에서의 GetTickCount와 같은 역활.
	QueryPerformanceCounter(&m_LastTime);
	QueryPerformanceCounter(&m_FixTime);

	QueryPerformanceFrequency(&m_CpuTick); // 1초에 cpu가 카운팅 할 수 있는 숫자를 저장.

	return NOERROR;
}

void CTimer::Compute_TimeDelta(void) // 매 프레임마다 호출 됨.
{
	QueryPerformanceCounter(&m_FrameTime);

	m_fTimeDelta = _float(m_FrameTime.QuadPart - m_LastTime.QuadPart) / _float(m_CpuTick.QuadPart);

	m_LastTime = m_FrameTime;

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick); // 1초에 cpu가 카운팅 할 수 있는 숫자를 저장.
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
		QueryPerformanceFrequency(&m_CpuTick); // 1초에 cpu가 카운팅 할 수 있는 숫자를 저장.
		m_FixTime = m_FrameTime;
		return true;
	}

	return false;
}

CTimer * CTimer::Create(void)
{
	CTimer*		pInstance = new CTimer;

	// FAILED : ()안에 결과로 받은 hr가 0보다 작을 경우에 조건문안으로 들어오기위한 매크로다.
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
