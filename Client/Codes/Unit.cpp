#include "stdafx.h"
#include "Unit.h"

CUnit::CUnit(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	ZeroMemory(&m_tUnitInfo, sizeof(UNITINFO));
}

CUnit::CUnit(const CUnit & rhs)
	: CGameObject(rhs)
	, m_tUnitInfo(rhs.m_tUnitInfo)
	, m_isDeadAni(false)
	, m_isAttacked(true)
{
}

void CUnit::Init_UnitInfo(const _float& fMaxHp, const _float& fAtt, const _float& fDef)
{
	m_tUnitInfo.fMaxHp = fMaxHp;
	m_tUnitInfo.fHp = fMaxHp;
	m_tUnitInfo.fAtt = fAtt;
	m_tUnitInfo.fDef = fDef;
}

const _matrix * CUnit::Get_WorldMatrix(void) const
{
	return nullptr;
}

const _matrix * CUnit::Get_matRotY(void) const
{
	return nullptr;
}

void CUnit::Add_Hp(const _float & fHpAmount)
{
	if (true == m_isAttacked)
	{
		if (fHpAmount < 0)
			m_tUnitInfo.fHp += fHpAmount + m_tUnitInfo.fDef;
		else
			m_tUnitInfo.fHp += fHpAmount;

		if (m_tUnitInfo.fMaxHp < m_tUnitInfo.fHp)
			m_tUnitInfo.fHp = m_tUnitInfo.fMaxHp;
	}
}

void CUnit::Add_Info(const UNITINFO * pInfo)
{
	m_tUnitInfo.fAtt += pInfo->fAtt;
	m_tUnitInfo.fDef += pInfo->fDef;
	m_tUnitInfo.fHp += pInfo->fHp;
	m_tUnitInfo.fMaxHp += pInfo->fMaxHp;
}

void CUnit::Set_Knock_Back(void)
{
}

void CUnit::Knock_Back(const _float & fTimeDelta)
{
}

_int CUnit::Update_GameObject(const _float & fTimeDelta)
{
	return _int();
}

HRESULT CUnit::SetUp_LightAndMaterialInfo_OnShader(LPD3DXEFFECT pEffect)
{
	//LightInfo
	if (FAILED(pEffect->SetVector("g_vLightDir", &_vec4(1.0f, -5.f, 1.0f, 0.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vLightDiffuse", &_vec4(0.8f, 0.8f, 0.8f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vLightAmvient", &_vec4(0.5f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vLightSpecular", &_vec4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(pEffect->SetVector("g_vMaterialDiffuse", &_vec4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vMaterialAmvient", &_vec4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vMaterialSpecular", &_vec4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetFloat("g_fSpecularPower", 20.f)))
		return E_FAIL;
}

void CUnit::Free(void)
{
	CGameObject::Free();
}
