#pragma once
#include "GameObject.h"
#include "Defines.h"

class CUnit : public CGameObject
{
protected:
	explicit CUnit(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUnit(const CUnit& rhs);
	virtual ~CUnit() = default;
protected:
	void Init_UnitInfo(const _float& fMaxHp, const _float& fAtt, const _float& fDef);
public:
	virtual const _matrix* Get_WorldMatrix(void) const;
	virtual const _matrix* Get_matRotY(void) const;
	virtual UNITINFO* Get_UnitInfo(void) {
		return &m_tUnitInfo;	}
	virtual _float* Get_SkillGage(void) {
		return &m_fGage;	}
public:
	virtual void Add_Hp(const _float& fHpAmount);
	virtual void Add_Info(const UNITINFO* pInfo);
	virtual void Set_Knock_Back(void);
	virtual void Knock_Back(const _float& fTimeDelta);
protected:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	HRESULT SetUp_LightAndMaterialInfo_OnShader(LPD3DXEFFECT pEffect);
protected:
	UNITINFO		m_tUnitInfo;
	_float			m_fGage = 0.f;
protected:
	_bool			m_isKnockBack = false;
	_vec3			m_vOriginPosForKnockBack = _vec3(0.f, 0.f, 0.f);
protected:
	_bool			m_isDeadAni = false;
	_bool			m_isAttacked = true;
public:
	virtual CGameObject* Clone(const _vec3* pPos = nullptr) { return this;  };
	virtual CGameObject* Clone(const _matrix& matWorld) { return this; };
public:
	virtual void Free(void);
};

