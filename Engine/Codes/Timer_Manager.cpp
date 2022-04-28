#include "Timer_Manager.h"
#include "Timer.h"

_USING(Engine)
_IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{
}


CTimer_Manager::~CTimer_Manager()
{
}

_float CTimer_Manager::Get_TimeDelta(const _tchar * pTimerTag)
{
	auto iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CTag_Finder(pTimerTag));

	if (iter == m_mapTimer.end())
		return 0.f;

	return iter->second->Get_TimeDelta();
}

HRESULT CTimer_Manager::Add_Timer(const _tchar * pTimerTag)
{
	auto iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CTag_Finder(pTimerTag));

	if (iter != m_mapTimer.end())
		return E_FAIL;

	CTimer*			pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;	

	m_mapTimer.insert(MAPTIMER::value_type(pTimerTag, pTimer));

	return NOERROR;
}

void CTimer_Manager::Compute_TimeDelta(const _tchar * pTimerTag)
{
	auto iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CTag_Finder(pTimerTag));

	if (iter == m_mapTimer.end())
		return;

	iter->second->Compute_TimeDelta();
}

_bool CTimer_Manager::Compute_TimeDelta(const _tchar* pTimerTag, const _float& fTime)
{
	auto iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CTag_Finder(pTimerTag));

	if (iter == m_mapTimer.end())
		return false;

	return iter->second->Compute_TimeDelta(fTime);
}

void CTimer_Manager::Free(void)
{
	for (auto& Pair : m_mapTimer)
	{
		Safe_Release(Pair.second);
	}

	m_mapTimer.clear();
}
