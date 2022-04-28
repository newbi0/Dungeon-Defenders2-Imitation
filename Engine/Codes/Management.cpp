#include "Management.h"
#include "Scene.h"

_USING(Engine)
_IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
{
}

HRESULT CManagement::SetUp_CurrentScene(CScene * pScene)
{
	_ulong dwRefCnt = 0;

	if (nullptr != m_pScene)
	{
		if (dwRefCnt = Safe_Release(m_pScene))
		{
			_MSG_BOX("Change Scene Failed");
			return E_FAIL;
		}
	}
	m_pScene = pScene;

	return NOERROR;
}

_int CManagement::Update_CurrentScene(const _float & fTimeDelta)
{
	if (nullptr == m_pScene)
		return _EXIT;

	m_pScene->Update_Scene(fTimeDelta);

	m_pScene->LastUpdate_Scene(fTimeDelta);

	return _RUN;
}

void CManagement::Render_CurrentScene(void)
{
	if (nullptr == m_pScene)
		return;


	m_pScene->Render_Scene();
}

void CManagement::Free(void)
{
	Safe_Release(m_pScene);
}
