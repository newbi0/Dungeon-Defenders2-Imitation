#include "Layer.h"
#include "Component.h"
#include "Collider.h"

_USING(Engine)

CLayer::CLayer()
{
}

CGameObject * CLayer::Get_GameObject(const _uint & iIndex)
{
	if (0 == m_ObjectList.size())
		return nullptr;
	auto iter = m_ObjectList.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return *iter;	
}

CGameObject * CLayer::Get_GameObject_ApplyToPosition(const _vec3 * pPos)
{
	for (auto pGameObject : m_ObjectList)
	{
		_vec3	vPos = *((CTransform*)pGameObject->Get_Component(L"Com_Transform"))->Get_Infomation(CTransform::INFO_POSITION);
		if (vPos == *pPos)
		{
			return pGameObject;
		}
	}
	return nullptr;
}

CGameObject * CLayer::Get_LastGameObject(void)
{
	_uint	iSize = m_ObjectList.size();

	auto iter = m_ObjectList.begin();
	for (_uint i = 0; i < iSize - 1; ++i)
		++iter;

	return *iter;
}

const CComponent * CLayer::Get_Component(const _tchar * pComponentTag, const _uint & iIndex)
{
	if (0 == m_ObjectList.size())
		return nullptr;
	auto iter = m_ObjectList.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_Component(pComponentTag);
}

const _bool CLayer::Get_isHitZone(void)
{
	for (auto& iter : m_ObjectList)
	{
		if (true == iter->Get_isHitZone() && true == iter->Get_isHitZoneFull())
			return true;
	}

	return false;
}

void CLayer::Set_HitZone(const CGameObject::HITZONETYPE & eType, const _uint& iSCENEID)
{
	for (auto& iter : m_ObjectList)
	{
		if (true == iter->Get_isHitZone())
		{
			iter->Set_HitZoneType(eType, iSCENEID);
		}
	}
}

HRESULT CLayer::Ready_Layer(void)
{
	return NOERROR;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_ObjectList.push_back(pGameObject);

	return NOERROR;
}

_int CLayer::Update_Layer(const _float & fTimeDelta)
{
	_int		iExitCode = 0;

	for (auto& pGameObject : m_ObjectList)
	{
		if (iExitCode = pGameObject->Update_GameObject(fTimeDelta))
			return iExitCode;		
	}
	return iExitCode;
}

_int CLayer::LastUpdate_Layer(const _float & fTimeDelta)
{
	_int		iExitCode = 0;

	for (auto& pGameObject : m_ObjectList)
	{
		if (iExitCode = pGameObject->LastUpdate_GameObject(fTimeDelta))
			return iExitCode;
	}
	return iExitCode;
}

_bool CLayer::Check_Collision_AABB(CCollider * pCollider, CGameObject** ppTarget)
{
	for (auto& pGameObject : m_ObjectList)
	{
		CCollider*	pDestCollider = (CCollider*)pGameObject->Get_Component(L"Com_Collider");
		if (pCollider == pDestCollider)
			continue;
		_bool isResult = pCollider->Collision_AABB(pDestCollider);
		if (true == isResult)
		{
			if (nullptr != ppTarget)
				*ppTarget = pGameObject;
			return isResult;
		}
	}

	return false;
}

_bool CLayer::Check_Collision_OBB(CCollider * pCollider, CGameObject** ppTarget, const _tchar* pColliderName)
{
	for (auto pGameObject : m_ObjectList)
	{
		CComponent* pTmp = pGameObject->Get_Component(pColliderName);
		CCollider** ppTargetCollider = (CCollider**)&pTmp;
		_bool isResult = pCollider->Collision_OBB(ppTargetCollider);
		if (true == isResult)
		{
			if(nullptr != ppTarget)
				*ppTarget = pGameObject;
			return isResult;
		}
	}

	return false;
}

void CLayer::Clear_DeadObject(void)
{
	auto iter = m_ObjectList.begin();
	for (; iter != m_ObjectList.end();)
	{
		if (true == (*iter)->Get_isDead())
		{
			iter = m_ObjectList.erase(iter);
		}
		else
			++iter;
	}
}

void CLayer::Clear_GameObject(void)
{
	for (auto& pGameObject : m_ObjectList)
	{
		pGameObject->Set_isDead(true);
	}
}

CLayer * CLayer::Create(void)
{
	CLayer*		pInstance = new CLayer();

	if (FAILED(pInstance->Ready_Layer()))
	{
		_MSG_BOX("CLayer Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLayer::Free(void)
{
	for ( auto& pGameObject : m_ObjectList )
	{
		Safe_Release(pGameObject);
	}
	m_ObjectList.clear();
}