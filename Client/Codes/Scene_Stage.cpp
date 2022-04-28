#include "stdafx.h"
#include "Defines.h"
#include "Scene_Stage.h"
#include "Texture.h"
#include "Input_Device.h"
#include "Management.h"
#include "Scene_Stage.h"
#include "Navigation.h"
#include "Turn.h"

CScene_Stage::CScene_Stage(LPDIRECT3DDEVICE9 pGraphic_Device, Engine::CRenderer * pRenderer)
	: CScene(pGraphic_Device, pRenderer)
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pTurn(CTurn::Get_Instance())
{
	m_pInput_Device->AddRef();
	m_pTurn->AddRef();
}

HRESULT CScene_Stage::Ready_Scene(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Layer_GameObject()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Component(void)
{
	CComponent*		pComponent = nullptr;

	m_pNaviCom = (CNavigation*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Navigation");
	if (nullptr == m_pNaviCom)
		return E_FAIL;
	return NOERROR;
}

HRESULT CScene_Stage::Ready_Prototype_GameObject(void)
{
	CGameObject*	pGameObject = nullptr;
	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_GameObject(void)
{
	return NOERROR;
}

_int CScene_Stage::Update_Scene(const _float & fTimeDelta)
{
	_int	iExitCode = 0;
 	iExitCode = m_pObject_Manager->Update_GameObject(fTimeDelta);

	m_pTurn->Update_Turn(fTimeDelta);
	
	if (0x80000000 & iExitCode)
		return iExitCode;

	return iExitCode;
}

_int CScene_Stage::LastUpdate_Scene(const _float & fTimeDelta)
{
	_int	iExitCode = 0;
	iExitCode = m_pObject_Manager->LastUpdate_GameObject(fTimeDelta);

	m_pObject_Manager->Clear_DeadObject();

	if (0x80000000 & iExitCode)
		return iExitCode;

	return iExitCode;
}

void CScene_Stage::Render_Scene(void)
{
	m_pRenderer->Render_Renderer();
	
	if(0x80 & m_pInput_Device->Get_KeyState(DIK_Z))
		m_pNaviCom->Render_Navigation();
}

CScene_Stage * CScene_Stage::Create(LPDIRECT3DDEVICE9 pGraphic_Device, Engine::CRenderer * pRenderer)
{
	CScene_Stage* pInstance = new CScene_Stage(pGraphic_Device, pRenderer);

	if (FAILED(pInstance->Ready_Scene()))
	{
		_MSG_BOX("CScene_Stage Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Stage::Free(void)
{
	m_pObject_Manager->Clear_GameObject(SCENE_STAGE);
	m_pComponent_Manager->Clear_Component(SCENE_STAGE);

	Safe_Release(m_pInput_Device);
	Safe_Release(m_pNaviCom);
	Safe_Release(m_pTurn);

	CScene::Free();
}
