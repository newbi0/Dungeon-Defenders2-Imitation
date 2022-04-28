#include "Skill.h"

CSkill::CSkill(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CSkill::CSkill(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkill::Ready_GameObject_Prototype(void)
{
	return NOERROR;
}

HRESULT CSkill::Ready_GameObject_Clone(const _vec3 * pPos)
{
	return NOERROR;
}

HRESULT CSkill::Ready_GameObject_Clone(const _matrix & matWorld)
{
	return NOERROR;
}

_int CSkill::Update_GameObject(const _float & fTimeDelta)
{
	return _int();
}

_int CSkill::LastUpdate_GameObject(const _float & fTimeDelta)
{
	return _int();
}

void CSkill::Render_GameObject(void)
{
}

void CSkill::Free(void)
{
	CGameObject::Free();
}
