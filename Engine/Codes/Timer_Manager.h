#pragma once

#include "Engine_Defines.h"
#include "Base.h"

_BEGIN(Engine)

class CTimer;
class _ENGINE_DLL CTimer_Manager : public CBase
{
	_DECLARE_SINGLETON(CTimer_Manager)
private:
	explicit CTimer_Manager();
	virtual ~CTimer_Manager();
public: // Getter
	_float Get_TimeDelta(const _tchar* pTimerTag);
public:
	HRESULT Add_Timer(const _tchar* pTimerTag);
	void Compute_TimeDelta(const _tchar* pTimerTag);
	_bool Compute_TimeDelta(const _tchar* pTimerTag, const _float& fTime);
private:
	unordered_map<const _tchar*, CTimer*>			m_mapTimer;
	typedef unordered_map<const _tchar*, CTimer*>	MAPTIMER;
private:
	virtual void Free(void);

};

_END

