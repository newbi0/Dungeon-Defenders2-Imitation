#pragma once
#include "GameObject.h"
#include "Defines.h"

class CUI : public CGameObject
{
protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUI(const CUI& rhs);
	virtual ~CUI() = default;
public:
	_bool Get_SeeAble(void) {
		return m_isSee;	}
public:
	virtual void Set_SeeAble(_bool isSeeAble);
protected:
	_bool					m_isSee = false;
public:
	virtual void Free(void);
};

