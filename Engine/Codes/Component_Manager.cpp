#include "Component_Manager.h"

_USING(Engine)
_IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
	: m_pMapComponent(nullptr)
{
}

HRESULT CComponent_Manager::Reserve_Container_Size(const _uint & iMaxSize)
{
	if (nullptr != m_pMapComponent)
		return E_FAIL;

	m_pMapComponent = new MAPCOMPONENT[iMaxSize];
	m_pVecTag = new VECTAG[iMaxSize];
	
	m_iMaxSize = iMaxSize;

	return NOERROR;
}

HRESULT CComponent_Manager::Add_Component(const _uint & iSceneIdx, const _tchar * pComponentTag, CComponent * pComponent)
{
	if (iSceneIdx >= m_iMaxSize)
		return E_FAIL;

	if (nullptr == m_pMapComponent)
		return E_FAIL;

	auto iter = find_if(m_pMapComponent[iSceneIdx].begin(), m_pMapComponent[iSceneIdx].end(), CTag_Finder(pComponentTag));
	if (iter != m_pMapComponent[iSceneIdx].end())
		return E_FAIL;

	m_pMapComponent[iSceneIdx].insert(MAPCOMPONENT::value_type(pComponentTag, pComponent));
	m_pVecTag[iSceneIdx].push_back(pComponentTag);

	return NOERROR;
}

CComponent * CComponent_Manager::Clone_Component(const _uint & iSceneIdx, const _tchar * pComponentTag)
{
	if (iSceneIdx >= m_iMaxSize)
		return nullptr;

	if (nullptr == m_pMapComponent)
		return nullptr;

	auto iter = find_if(m_pMapComponent[iSceneIdx].begin(), m_pMapComponent[iSceneIdx].end(), CTag_Finder(pComponentTag));
	if (iter == m_pMapComponent[iSceneIdx].end())
		return nullptr;

	return iter->second->Clone();	
}

HRESULT CComponent_Manager::Clear_Component(const _uint & iSceneIdx)
{
	if (iSceneIdx >= m_iMaxSize)
		return E_FAIL;

	for (auto& Pair : m_pMapComponent[iSceneIdx])
		Safe_Release(Pair.second);

	m_pMapComponent[iSceneIdx].clear();

	//for (auto& iter : m_pVecTag[iSceneIdx])
	//{
	//	Safe_Release(iter);
	//}
	//m_pVecTag[iSceneIdx].clear();

	return NOERROR;
}

void CComponent_Manager::Free(void)
{
	for (size_t i = 0; i < m_iMaxSize; ++i)
	{
		for (auto& Pair : m_pMapComponent[i])
		{
			Safe_Release(Pair.second);
		}
		m_pMapComponent[i].clear();
	}

	//for (size_t i = 0; i < m_iMaxSize; ++i)
	//{
	//	for (auto& iter : m_pVecTag[i])
	//	{
	//		Safe_Release(iter);
	//	}
	//	m_pVecTag[i].clear();
	//}

	Safe_Delete_Array(m_pMapComponent);
	Safe_Delete_Array(m_pVecTag);
}



