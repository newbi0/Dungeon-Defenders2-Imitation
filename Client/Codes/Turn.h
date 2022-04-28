#pragma once
#include "Base.h"

namespace Engine
{
	class CObject_Manager;
	class CSound_Device;
}


class CTurn final :
	public CBase
{
	_DECLARE_SINGLETON(CTurn)
public:
	explicit CTurn(void);
	virtual ~CTurn(void) = default;
public:
	enum eTURN {TURN_BUILD, TURN_WAVE, TURN_WIN, TURN_END};
public:
	void Set_Turn(eTURN eTrunState) {
		m_eCurrentTurn = eTrunState;	}
public:
	const _uint Get_Turn(void) const {
		return m_eCurrentTurn;	}
	const _uint Get_NextTurn(void) const {
		return m_iNextTurn;	}
public:
	HRESULT Ready_Turn(const _vec3* pZenPointArray, const _uint& iNumArray);
public:
	_int Update_Turn(_float fTimeDelta);
public:
	HRESULT Start_Wave(void);
	HRESULT Start_Wave_1(void);
	HRESULT Start_Wave_Boss(void);
private:
	eTURN				m_eCurrentTurn = TURN_BUILD;
	_uint				m_iNextTurn = 1;
private:
	_uint				m_iZenArrayNum = 0;
	_vec3*				m_pZenPoint = nullptr;
private:
	CObject_Manager*	m_pObject_Manager = nullptr;
	CSound_Device*		m_pSound_Device = nullptr;
private:
	virtual void Free(void);
};