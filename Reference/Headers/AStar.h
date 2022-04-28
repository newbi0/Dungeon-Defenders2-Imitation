#pragma once

#include "Component.h"

_BEGIN(Engine)

class CNavigation;
class _ENGINE_DLL CAStar : public CComponent
{

private:
	explicit CAStar(LPDIRECT3DDEVICE9 pGraphic_Device, CNavigation* pNaviCom);
	explicit CAStar(const CAStar& rhs);
	virtual ~CAStar() = default;
public:
	list<_vec3> Make_Rute(const _uint& iStartIdx, const _vec3& vStartPos, const _vec3& vEndPos);
	NODE Compute_Cost(const _vec3& vStartPos, const _vec3& vEndPos);
private:
	list<_uint>			m_listOpen;
	list<_uint>			m_listClose;
	list<_vec3>			m_listBest;
private:
	CNavigation*	m_pNavigationCom = nullptr;
public:
	virtual CComponent* Clone(void);
	static CAStar* Create(LPDIRECT3DDEVICE9 pGraphic_Device, CNavigation* pNaviCom);
private:
	HRESULT Ready_Astar(void);
	virtual void Free(void);
};

_END