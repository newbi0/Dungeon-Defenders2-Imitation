#pragma once

#include "Engine_Defines.h"

_BEGIN(Engine)

class _ENGINE_DLL CBase abstract
{
protected:
	explicit CBase(void);
	virtual ~CBase(void) = default;
public:
	virtual _ulong AddRef(void);
	virtual _ulong Release(void);
protected:
	_ulong	m_dwRefCnt = 0;
protected:
	virtual void Free(void);
};

_END