#pragma once

#include "Engine_Defines.h"
#include "Base.h"

_BEGIN(Engine)

class _ENGINE_DLL CComponent abstract : public CBase
{
protected:
	explicit CComponent(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;
protected:
	LPDIRECT3DDEVICE9			m_pGraphic_Device;
	_bool						m_isClone = false;
public:
	virtual CComponent* Clone(void) = 0;
	virtual void Free(void);
};

_END