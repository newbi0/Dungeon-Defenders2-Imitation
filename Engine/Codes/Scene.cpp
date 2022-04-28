#include "Scene.h"

_USING(Engine)

CScene::CScene(LPDIRECT3DDEVICE9 pGraphic_Device, CRenderer* pRenderer)
	: m_pGraphic_Device(pGraphic_Device)
	, m_pRenderer(pRenderer)
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
{
	m_pGraphic_Device->AddRef();
	m_pRenderer->AddRef();
	m_pObject_Manager->AddRef();
	m_pComponent_Manager->AddRef();
}


CScene::~CScene()
{
}

HRESULT CScene::Ready_Scene(void)
{

	return NOERROR;
}

_int CScene::Update_Scene(const _float & fTimeDelta)
{
	m_pObject_Manager->Update_GameObject(fTimeDelta);
	
	return _RUN;
}

_int CScene::LastUpdate_Scene(const _float & fTimeDelta)
{
	m_pObject_Manager->LastUpdate_GameObject(fTimeDelta);

	return _int();
}

void CScene::Render_Scene(void)
{
	if (nullptr == m_pRenderer)
		return;

	m_pRenderer->Render_Renderer();

}

void CScene::Free(void)
{
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGraphic_Device);
}
