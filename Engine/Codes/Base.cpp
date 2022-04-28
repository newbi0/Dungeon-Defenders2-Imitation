#include "Base.h"

_USING(Engine)

CBase::CBase()
{
}

_ulong CBase::AddRef(void)
{
	return ++m_dwRefCnt;
}

_ulong CBase::Release(void)
{
	if (0 == m_dwRefCnt)
	{
		Free();
		delete this;
		return 0;
	}

	return m_dwRefCnt--;
}

void CBase::Free(void)
{
}
