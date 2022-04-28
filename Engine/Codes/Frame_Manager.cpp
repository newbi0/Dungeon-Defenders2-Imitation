
#include "Frame_Manager.h"
#include "Frame.h"

_USING(Engine)
_IMPLEMENT_SINGLETON(CFrame_Manager)

CFrame_Manager::CFrame_Manager()
{
}


CFrame_Manager::~CFrame_Manager()
{
}

HRESULT CFrame_Manager::Add_Frame(const _tchar * pFrameTag, const _float& fRenderCall)
{
	CFrame*	pFrame_Find = Find_Frame(pFrameTag);

	if (nullptr != pFrame_Find)
		return E_FAIL;

	CFrame*		pFrame = CFrame::Create(fRenderCall);
	if (nullptr == pFrame)
		return E_FAIL;

	m_mapFrame.insert(MAPFRAME::value_type(pFrameTag, pFrame));

	return NOERROR;
}

_bool CFrame_Manager::Permit_Call(const _tchar * pFrameTag, const _float & fTimeDelta)
{
	CFrame*	pFrame_Find = Find_Frame(pFrameTag);

	if (nullptr == pFrame_Find)
		return false;

	return pFrame_Find->Permit_Call(fTimeDelta);	
}

CFrame * CFrame_Manager::Find_Frame(const _tchar* pFrameTag)
{
	auto iter = find_if(m_mapFrame.begin(), m_mapFrame.end(), CTag_Finder(pFrameTag));

	if (iter == m_mapFrame.end())
		return nullptr;

	return iter->second;
}

void CFrame_Manager::Free(void)
{
	for (auto& Pair : m_mapFrame)
	{
		Safe_Release(Pair.second);
	}

	m_mapFrame.clear();
}
