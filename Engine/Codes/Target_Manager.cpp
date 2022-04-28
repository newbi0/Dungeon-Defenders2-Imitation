#include "Target_Manager.h"
#include "Target.h"

_USING(Engine)
_IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{

}

HRESULT CTarget_Manager::Add_Target(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pTargetTag, const _ulong & dwSizeX, const _ulong & dwSizeY, D3DFORMAT Format, D3DXCOLOR Color)
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if (nullptr != pTarget)
		return E_FAIL;

	pTarget = CTarget::Create(pGraphic_Device, dwSizeX, dwSizeY, Format, Color);
	if (nullptr == pTarget)
		return E_FAIL;

	m_mapTarget.insert(MAPTARGET::value_type(pTargetTag, pTarget));

	return NOERROR;
}

HRESULT CTarget_Manager::Ready_DebugBuffer(const _tchar * pTargetTag, const _float & fX, const _float & fY, const _float & fSizeX, const _float & fSizeY)
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	if (FAILED(pTarget->Ready_DebugBuffer(fX, fY, fSizeX, fSizeY)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		list<CTarget*>		MRTList;
		MRTList.push_back(pTarget);
		m_mapMRT.insert(MAPMRT::value_type(pMRTTag, MRTList));
	}
	else
		pMRTList->push_back(pTarget);

	return NOERROR;
}

HRESULT CTarget_Manager::Begin_MRT(const _tchar * pMRTTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pTarget : *pMRTList)
	{
		pTarget->Clear_Target();
	}

	_ulong		dwIndex = 0;

	for (auto& pTarget : *pMRTList)
	{
		pTarget->SetUp_OnGraphicDevice(dwIndex++);
	}

	return NOERROR;
}

HRESULT CTarget_Manager::End_MRT(const _tchar * pMRTTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	_ulong		dwIndex = 0;

	for (auto& pTarget : *pMRTList)
	{
		pTarget->Release_OnGraphicDevice(dwIndex++);
	}

	return NOERROR;
}

void CTarget_Manager::Render_DebugBuffer(const _tchar * pMRTTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return;

	for (auto& pTarget : *pMRTList)
	{
		pTarget->Render_DebugBuffer();
	}
}

HRESULT CTarget_Manager::SetUp_OnShader(LPD3DXEFFECT pEffect, const _tchar * pTargetTag, const char * pConstantName)
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->SetUp_OnShader(pEffect, pConstantName);
}

HRESULT CTarget_Manager::SetUp_OnShader_Matrix(LPD3DXEFFECT pEffect, const _tchar * pTargetTag, const char * pConstantName, const _matrix & matValue)
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->SetUp_OnShader_Matrix(pEffect, pConstantName, matValue);
}

CTarget * CTarget_Manager::Find_Target(const _tchar * pTargetTag)
{
	auto iter = find_if(m_mapTarget.begin(), m_mapTarget.end(), CTag_Finder(pTargetTag));

	if (iter == m_mapTarget.end())
		return nullptr;

	return iter->second;
}

list<CTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto iter = find_if(m_mapMRT.begin(), m_mapMRT.end(), CTag_Finder(pMRTTag));

	if (iter == m_mapMRT.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free(void)
{
	for (auto& Pair : m_mapTarget)
	{
		Safe_Release(Pair.second);
	}
	m_mapTarget.clear();
}
