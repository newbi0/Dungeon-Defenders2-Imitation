#pragma once

#include "Engine_Defines.h"
#include "Base.h"

_BEGIN(Engine)

class CTimer : public CBase
{
public:
	explicit CTimer();
	virtual ~CTimer();
public: // Getter
	_float Get_TimeDelta(void) const {
		return m_fTimeDelta;}
public:
	HRESULT Ready_Timer(void);
	void Compute_TimeDelta(void);
	_bool Compute_TimeDelta(const _float& fTime);
private:
	_float			m_fTimeDelta;
private:
	LARGE_INTEGER	m_FrameTime;
	LARGE_INTEGER	m_FixTime;
	LARGE_INTEGER	m_LastTime;
	LARGE_INTEGER	m_CpuTick;
public:
	static CTimer* Create(void);
private:
	virtual void Free(void);
};

_END


