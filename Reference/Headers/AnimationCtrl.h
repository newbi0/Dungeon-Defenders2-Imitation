#pragma once
#include "Base.h"

_BEGIN(Engine)

class CAnimationCtrl : public CBase
{
private:
	explicit CAnimationCtrl(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CAnimationCtrl(const CAnimationCtrl& rhs);
	virtual ~CAnimationCtrl(void) = default;
public:
	LPD3DXANIMATIONCONTROLLER* Get_AnimationCtrl(void) {
		return &m_pAniCtrl;	}
	const _uint Get_CurrentAniIdx(void) const {
		return m_iCurrentAniSetIndex;	}
public:
	void Set_AdjustPeriod(const _double& dPeriod) {
		m_dAdjustedPeriod = dPeriod;	}
public:
	HRESULT Ready_AnimationCtrl(void);
	//-----------------------------------
	void SetUp_AnimationSet(const _uint& iIndex, const _float& fWeight);
	void Play_Animation(const _float& fTimeDelta);
	_bool is_Finished(void);
private:
	_uint			m_iCurrentAniSetIndex = 0;
	_uint			m_iMaxNumAniSet = 0;		//애니메이션 총 개수
	_uint			m_iCurrentTrack = 0;
	_uint			m_iNewTrack = 0;
	_double			m_dPeriod = 0.0;
	_double			m_dAdjustedPeriod = 0.0;
	_float			m_fTimeAcc = 0.f;
private:
	LPDIRECT3DDEVICE9				m_pGraphic_Device = nullptr;
	LPD3DXANIMATIONCONTROLLER		m_pAniCtrl = nullptr;
public:
	static CAnimationCtrl* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	static CAnimationCtrl* Clone(const CAnimationCtrl& rhs);
private:
	virtual void Free(void);
};

_END