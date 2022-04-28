#include "Component_Manager.h"
#include "Object_Manager.h"
#include "Component.h"

_USING(Engine)
_IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
	: m_pMapLayer(nullptr)
	, m_pMapObject_Prototype(nullptr)
{
}

CGameObject * CObject_Manager::Get_GameObject(const _uint & iSceneID, const _tchar * pLayerTag, const _uint & iIndex)
{
	if (iSceneID >= m_iMaxSize)
		return nullptr;

	auto iter = find_if(m_pMapLayer[iSceneID].begin(), m_pMapLayer[iSceneID].end(), CTag_Finder(pLayerTag));

	if (iter == m_pMapLayer[iSceneID].end())
		return nullptr;

	return iter->second->Get_GameObject(iIndex);
}

CGameObject * CObject_Manager::Get_GameObject_ApplyToPosition(const _uint & iSceneID, const _tchar * pLayerTag, const _vec3 * pPos)
{
	if (iSceneID >= m_iMaxSize)
		return nullptr;

	auto iter = find_if(m_pMapLayer[iSceneID].begin(), m_pMapLayer[iSceneID].end(), CTag_Finder(pLayerTag));

	if (iter == m_pMapLayer[iSceneID].end())
		return nullptr;

	return iter->second->Get_GameObject_ApplyToPosition(pPos);
}

CGameObject * CObject_Manager::Get_LastGameObject(const _uint & iSceneID, const _tchar * pLayerTag)
{
	if (iSceneID >= m_iMaxSize)
		return nullptr;

	auto iter = find_if(m_pMapLayer[iSceneID].begin(), m_pMapLayer[iSceneID].end(), CTag_Finder(pLayerTag));

	if (iter == m_pMapLayer[iSceneID].end())
		return nullptr;

	return iter->second->Get_LastGameObject();
}

const CComponent * CObject_Manager::Get_Component(const _uint & iSceneID, const _tchar * pLayerTag, const _tchar * pComponentTag, const _uint & iIndex)
{
	if (iSceneID >= m_iMaxSize)
		return nullptr;

	auto iter = find_if(m_pMapLayer[iSceneID].begin(), m_pMapLayer[iSceneID].end(), CTag_Finder(pLayerTag));

	if (iter == m_pMapLayer[iSceneID].end())
		return nullptr;

	return iter->second->Get_Component(pComponentTag, iIndex);
}

const _uint CObject_Manager::Get_GameObjectNum(const _uint & iSceneID, const _tchar * pLayerTag)
{
	auto iter = find_if(m_pMapLayer[iSceneID].begin(), m_pMapLayer[iSceneID].end(), CTag_Finder(pLayerTag));

	if (iter == m_pMapLayer[iSceneID].end())
		return 0;

	return iter->second->Get_GameObjectNum();
}

const _bool CObject_Manager::Get_isHitZone(void)
{
	for (_uint i = 0; i < m_iMaxSize; ++i)
	{
		for (auto& iter : m_pMapLayer[i])
		{
			if (true == iter.second->Get_isHitZone())
				return true;
		}
	}

	return false;
}

void CObject_Manager::Set_HitZone(const CGameObject::HITZONETYPE & eType, const _uint& iSCENEID)
{
	for (auto& iter : m_pMapLayer[iSCENEID])
	{
		iter.second->Set_HitZone(eType, iSCENEID);
	}
}

HRESULT CObject_Manager::Reserve_Container_Size(const _uint & iMaxSize)
{
	if (nullptr != m_pMapObject_Prototype)
		return E_FAIL;

	m_pMapObject_Prototype = new MAPOBJECT_PROTOTYPE[iMaxSize];
	m_pVecObjectTag = new VECOBJECTTAG[iMaxSize];
	m_pVecLoad = new VECLOAD[iMaxSize];

	if (nullptr != m_pMapLayer)
		return E_FAIL;

	m_pMapLayer = new MAPLAYER[iMaxSize];
	
	m_iMaxSize = iMaxSize;

	return NOERROR;
}

HRESULT CObject_Manager::Add_GameObject_Prototype(const _uint & iSceneIdx, const _tchar * pObjectTag, CGameObject * pGameObject_Prototype)
{
	if (nullptr == pGameObject_Prototype
		|| nullptr == m_pMapObject_Prototype)
		return E_FAIL;

	auto iter = find_if(m_pMapObject_Prototype[iSceneIdx].begin(), m_pMapObject_Prototype[iSceneIdx].end(), CTag_Finder(pObjectTag));

	if (iter != m_pMapObject_Prototype[iSceneIdx].end())
		return E_FAIL;

	m_pMapObject_Prototype[iSceneIdx].insert(MAPOBJECT_PROTOTYPE::value_type(pObjectTag, pGameObject_Prototype));
	m_pVecObjectTag[iSceneIdx].push_back(pObjectTag);

	return NOERROR;
}

HRESULT CObject_Manager::Add_GameObject(const _uint& iSceneIdx, const _tchar* pLayerTag, const _tchar* pObjectTag, const _vec3* pPos)
{
	if (iSceneIdx >= m_iMaxSize)
		return E_FAIL;

	if (nullptr == m_pMapLayer)
		return E_FAIL;

	auto iter_Prototype = find_if(m_pMapObject_Prototype[iSceneIdx].begin(), m_pMapObject_Prototype[iSceneIdx].end(), CTag_Finder(pObjectTag));
	if (iter_Prototype == m_pMapObject_Prototype[iSceneIdx].end())
		return E_FAIL;

	CGameObject*	pGameObject = nullptr;
	if(nullptr == pPos)
		pGameObject = iter_Prototype->second->Clone();
	else
		pGameObject = iter_Prototype->second->Clone(pPos);

	if (nullptr == pGameObject)
		return E_FAIL;

	auto iter_Layer = find_if(m_pMapLayer[iSceneIdx].begin(), m_pMapLayer[iSceneIdx].end(), CTag_Finder(pLayerTag));
	if (iter_Layer == m_pMapLayer[iSceneIdx].end())
	{
		CLayer*		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pMapLayer[iSceneIdx].insert(MAPLAYER::value_type(pLayerTag, pLayer));
	}
	else	
		iter_Layer->second->Add_GameObject(pGameObject);

	return NOERROR;
}

HRESULT CObject_Manager::Add_GameObject(const _uint & iSceneIdx, const _tchar * pLayerTag, const _tchar * pObjectTag, _matrix matWorld)
{
	if (iSceneIdx >= m_iMaxSize)
		return E_FAIL;

	if (nullptr == m_pMapLayer)
		return E_FAIL;

	auto iter_Prototype = find_if(m_pMapObject_Prototype[iSceneIdx].begin(), m_pMapObject_Prototype[iSceneIdx].end(), CTag_Finder(pObjectTag));
	if (iter_Prototype == m_pMapObject_Prototype[iSceneIdx].end())
		return E_FAIL;

	auto iter_Layer = find_if(m_pMapLayer[iSceneIdx].begin(), m_pMapLayer[iSceneIdx].end(), CTag_Finder(pLayerTag));
	if (iter_Layer == m_pMapLayer[iSceneIdx].end())
	{
		CLayer*		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		CGameObject*		pGameObject = iter_Prototype->second->Clone(matWorld);
		if (nullptr == pGameObject)
			return E_FAIL;
		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;

		m_pMapLayer[iSceneIdx].insert(MAPLAYER::value_type(pLayerTag, pLayer));
	}
	else
		iter_Layer->second->Add_GameObject(iter_Prototype->second->Clone(matWorld));

	return NOERROR;
}

_int CObject_Manager::Update_GameObject(const _float & fTimeDelta)
{
	_int		iExitCode = 0;

	for (size_t i = 0; i < m_iMaxSize; ++i)
	{
		for (auto& Pair : m_pMapLayer[i])
		{
			if (iExitCode = Pair.second->Update_Layer(fTimeDelta))
				return iExitCode;
		}	
	}	
	return iExitCode;
}

_int CObject_Manager::LastUpdate_GameObject(const _float & fTimeDelta)
{
	_int		iExitCode = 0;

	for (size_t i = 0; i < m_iMaxSize; ++i)
	{
		for (auto& Pair : m_pMapLayer[i])
		{
			if (iExitCode = Pair.second->LastUpdate_Layer(fTimeDelta))
				return iExitCode;
		}
	}
	return iExitCode;
}

_bool CObject_Manager::Check_Collision_AABB(const _uint & iSceneIdx, CCollider * pCollider, const _tchar * pLayerTag, CGameObject** ppTarget)
{
	auto iter_Layer = find_if(m_pMapLayer[iSceneIdx].begin(), m_pMapLayer[iSceneIdx].end(), CTag_Finder(pLayerTag));
	if (iter_Layer == m_pMapLayer[iSceneIdx].end())
		return false;

	return iter_Layer->second->Check_Collision_AABB(pCollider, ppTarget);
}

_bool CObject_Manager::Check_Collision_OBB(const _uint & iSceneIdx, CCollider * pCollider, const _tchar * pLayerTag, CGameObject** ppTarget, const _tchar* pColliderName)
{
	if (nullptr == pCollider)
		return false;

	auto iter_Layer = find_if(m_pMapLayer[iSceneIdx].begin(), m_pMapLayer[iSceneIdx].end(), CTag_Finder(pLayerTag));
	if (iter_Layer == m_pMapLayer[iSceneIdx].end())
		return false;

	return iter_Layer->second->Check_Collision_OBB(pCollider, ppTarget, pColliderName);
}

void CObject_Manager::Load_Object(const _tchar * pFilePath, const _uint& SceneID)
{
	HANDLE	hFile;

	hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	_ulong dwByte = 0;

	for (;;)
	{
		LOAD_INFO*	pInfo = new LOAD_INFO;

		ReadFile(hFile, pInfo, sizeof(LOAD_INFO), &dwByte, 0);
		if (0 == dwByte)
			break;

		Add_GameObject(SceneID, pInfo->szLayerTag, pInfo->szObjectTag, pInfo->matWorld);

 		m_pVecLoad[SceneID].push_back(pInfo);
	}

	CloseHandle(hFile);
}

void CObject_Manager::Clear_DeadObject(void)
{
	for (_uint i = 0; i < m_iMaxSize; ++i)
	{
		for (auto& Pair : m_pMapLayer[i])
			Pair.second->Clear_DeadObject();
	}
}

HRESULT CObject_Manager::Clear_GameObject(const _uint & iSceneIdx)
{
	if (iSceneIdx >= m_iMaxSize)
		return E_FAIL;	

	for (auto& Pair : m_pMapLayer[iSceneIdx])	
		Safe_Release(Pair.second);	

	m_pMapLayer[iSceneIdx].clear();

	for (auto& Pair : m_pMapObject_Prototype[iSceneIdx])
		Safe_Release(Pair.second);

	m_pMapObject_Prototype[iSceneIdx].clear();

	auto iter = m_pVecLoad[iSceneIdx].begin();
	for (iter; iter != m_pVecLoad[iSceneIdx].end();)
	{
		Safe_Delete(*iter);
		iter = m_pVecLoad[iSceneIdx].erase(iter);
	}
	m_pVecLoad[iSceneIdx].clear();

	//for (auto& iter : m_pVecObjectTag[iSceneIdx])
	//	Safe_Release(iter);
	//m_pVecObjectTag[iSceneIdx].clear();

	return NOERROR;
}

HRESULT CObject_Manager::Clear_GameObject(const _uint & iSceneIdx, const _tchar * pLayerTag)
{
	auto	Pair = find_if(m_pMapLayer[iSceneIdx].begin(), m_pMapLayer[iSceneIdx].end(), CTag_Finder(pLayerTag));

	if (Pair == m_pMapLayer[iSceneIdx].end())
		return NOERROR;

	Pair->second->Clear_GameObject();

	return NOERROR;
}


void CObject_Manager::Free(void)
{
	for (size_t i = 0; i < m_iMaxSize; ++i)
	{
		for (auto& Pair : m_pMapLayer[i])
		{
			Safe_Release(Pair.second);
		}
		m_pMapLayer[i].clear();		
	}

	for (size_t i = 0; i < m_iMaxSize; i++)
	{
		for (auto& Pair : m_pMapObject_Prototype[i])
		{
			Safe_Release(Pair.second);
		}
		m_pMapObject_Prototype[i].clear();
	}

	for (size_t i = 0; i < m_iMaxSize; i++)
	{
		auto iter = m_pVecLoad[i].begin();
		for (iter; iter != m_pVecLoad[i].end();)
		{
			Safe_Delete(*iter);
			iter = m_pVecLoad[i].erase(iter);
		}
		m_pVecLoad[i].clear();
	}

	Safe_Delete_Array(m_pMapLayer);
	Safe_Delete_Array(m_pMapObject_Prototype);
	Safe_Delete_Array(m_pVecObjectTag);
	Safe_Delete_Array(m_pVecLoad);

}
