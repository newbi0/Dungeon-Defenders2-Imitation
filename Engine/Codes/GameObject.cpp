#include "GameObject.h"
#include "Component.h"
#include "Texture.h"

_USING(Engine)

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
{
	m_pGraphic_Device->AddRef();
	m_pComponent_Manager->AddRef();
	m_pInput_Device->AddRef();
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pGraphic_Device(rhs.m_pGraphic_Device)
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pInput_Device(rhs.m_pInput_Device)
	, m_fViewZ(rhs.m_fViewZ)
	, m_isHitZone(rhs.m_isHitZone)
	, m_eHitZoneType(rhs.m_eHitZoneType)
	, m_isCulling(false)
{
	m_pGraphic_Device->AddRef();
	m_pComponent_Manager->AddRef();
	m_pInput_Device->AddRef();

	for (auto& Pair : rhs.m_mapComponent)
	{
		CComponent* pComponent = Pair.second->Clone();
		m_mapComponent.insert(MAPCOMPONENT::value_type(Pair.first, pComponent));
	}			

	m_isClone = true;
}

CComponent * CGameObject::Get_Component(const _tchar * pComponentTag)
{
	CComponent* pComponent = Find_Component(pComponentTag);
	if(nullptr == pComponent)
		return nullptr;

	return pComponent;
}

void CGameObject::Set_HitZoneType(HITZONETYPE eType, const _uint& iSCENEID)
{
	m_eHitZoneType = eType;
	m_isHitZoneFull = false;

	if (TYPE_END == m_eHitZoneType || nullptr != m_pHitZomeTextureCom)
		Safe_Release(m_pHitZomeTextureCom);

	if (TYPE_CIRCLE == m_eHitZoneType)
	{
		m_pHitZomeTextureCom = (CTexture*)m_pComponent_Manager->Clone_Component(iSCENEID, L"Com_Texture_Effect_HitZone_Circle");
		if (nullptr == m_pHitZomeTextureCom)
			return;
		Engine::CGameObject::Add_Component(L"Com_HitZoneTexture", m_pHitZomeTextureCom);
	}
	else if (TYPE_DONUT == m_eHitZoneType)
	{
		m_pHitZomeTextureCom = (CTexture*)m_pComponent_Manager->Clone_Component(iSCENEID, L"Com_Texture_Effect_HitZone_Donut");
		if (nullptr == m_pHitZomeTextureCom)
			return;
		Engine::CGameObject::Add_Component(L"Com_HitZoneTexture", m_pHitZomeTextureCom);
	}
}

HRESULT CGameObject::Ready_GameObject_Prototype(void) // 원형 객체의 초기화
{
	return NOERROR;
}

HRESULT CGameObject::Ready_GameObject_Clone(const _vec3* pPos)
{
	return NOERROR;
}

HRESULT CGameObject::Ready_GameObject_Clone(const _matrix& matWorld)
{
	return NOERROR;
}

void CGameObject::Render_GameObject(void)
{
}

_int CGameObject::Update_GameObject(const _float & fTimeDelta)
{
	return _int();
}

_int CGameObject::LastUpdate_GameObject(const _float & fTimeDelta)
{
	return _int();
}

HRESULT CGameObject::Add_Component(const _tchar * pComponentTag, CComponent * pComponent)
{
	CComponent* pComponent_Find = Find_Component(pComponentTag);
	if (nullptr != pComponent_Find)
		return E_FAIL;

	m_mapComponent.insert(MAPCOMPONENT::value_type(pComponentTag, pComponent));
	pComponent->AddRef();

	return NOERROR;
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto& iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), Engine::CTag_Finder(pComponentTag));
	if (iter == m_mapComponent.end())
		return nullptr;
	
	return iter->second;
}


void CGameObject::Compute_ViewZ(const _vec3 * pPosition)
{
	_matrix			matView;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	_vec3		vCamPosition;
	memcpy(&vCamPosition, &matView.m[3][0], sizeof(_vec3));

	m_fViewZ = D3DXVec3Length(&(*pPosition - vCamPosition));
}

void CGameObject::Free(void)
{
	if (nullptr != m_pHitZomeTextureCom)
		Safe_Release(m_pHitZomeTextureCom);

	for (auto& Pair : m_mapComponent)
	{

		Safe_Release(Pair.second);
	}
	m_mapComponent.clear();

	Engine::Safe_Release(m_pInput_Device);
	Engine::Safe_Release(m_pComponent_Manager);
	Engine::Safe_Release(m_pGraphic_Device);
}
