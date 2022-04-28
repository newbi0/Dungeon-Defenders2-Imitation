#include "stdafx.h"
#include "Defines.h"
#include "Scene_Logo.h"
#include "Back_Logo.h"
#include "Texture.h"
#include "Input_Device.h"
#include "Management.h"
#include "Scene_Stage.h"
#include "Loading.h"
#include "Sound_Device.h"

CScene_Logo::CScene_Logo(LPDIRECT3DDEVICE9 pGraphic_Device, Engine::CRenderer * pRenderer)
	: CScene(pGraphic_Device, pRenderer)
	, m_pInput_Device(CInput_Device::Get_Instance())
{
	m_pInput_Device->AddRef();
}

HRESULT CScene_Logo::Ready_Scene(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Layer_GameObject()))
		return E_FAIL;

	CSound_Device*	pSoundDev = CSound_Device::Get_Instance();
	if (nullptr == pSoundDev)
		return E_FAIL;

	if (FAILED(pSoundDev->LoadWave(L"../Bin/Resource/Sound/01-Dungeon-Defenders.wav")))
		return E_FAIL;

	//pSoundDev->SoundPlay(0, 0);

	m_pLoading = CLoading::Create(m_pGraphic_Device, SCENE_STAGE);
	if (nullptr == m_pLoading)
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Logo::Ready_Component(void)
{
	CComponent*		pComponent = nullptr;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Logo/Start.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_LOGO, L"Com_Texture_Logo", pComponent)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Logo::Ready_Prototype_GameObject(void)
{
	CGameObject*	pGameObject = nullptr;

	pGameObject = CBack_Logo::Create(m_pGraphic_Device);
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_LOGO, L"Object_BackLogo", pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Logo::Ready_Layer_GameObject(void)
{
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_LOGO, L"Layer_BackLogo", L"Object_BackLogo")))
		return E_FAIL;

	return NOERROR;
}

_int CScene_Logo::Update_Scene(const _float & fTimeDelta)
{
	SetWindowText(g_hWnd, m_pLoading->Get_String());

	_int	iExitCode = 0;
	iExitCode = m_pObject_Manager->Update_GameObject(fTimeDelta);
	
	if (0x80000000 & iExitCode)
		return iExitCode;

	return iExitCode;
}

_int CScene_Logo::LastUpdate_Scene(const _float & fTimeDelta)
{
	_int	iExitCode = 0;
	iExitCode = m_pObject_Manager->LastUpdate_GameObject(fTimeDelta);

	if (0x80000000 & iExitCode)
		return iExitCode;

	if (0x80 & m_pInput_Device->Get_KeyState(DIK_RETURN)
		&& true == m_pLoading->Get_isLoadingFinish())
	{
		CScene*		pScene = CScene_Stage::Create(m_pGraphic_Device, m_pRenderer);
		if (FAILED(CManagement::Get_Instance()->SetUp_CurrentScene(pScene)))
		{
			_MSG_BOX("Failed Chang to Scene");
			return _EXIT;
		}

		//¹è°æÀ½¾Ç
		CSound_Device*	pSoundDev = CSound_Device::Get_Instance();
		if (nullptr == pSoundDev)
			return E_FAIL;

		pSoundDev->SoundStop(0);
		pSoundDev->SoundPlay(1, 0);
	}

	return iExitCode;
}

void CScene_Logo::Render_Scene(void)
{
	m_pRenderer->Render_Renderer();
}

CScene_Logo * CScene_Logo::Create(LPDIRECT3DDEVICE9 pGraphic_Device, Engine::CRenderer * pRenderer)
{
	CScene_Logo* pInstance = new CScene_Logo(pGraphic_Device, pRenderer);

	if (FAILED(pInstance->Ready_Scene()))
	{
		_MSG_BOX("CScene_Logo Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Logo::Free(void)
{
	m_pObject_Manager->Clear_GameObject(SCENE_LOGO);
	m_pComponent_Manager->Clear_Component(SCENE_LOGO);

	Safe_Release(m_pInput_Device);
	Safe_Release(m_pLoading);

	CScene::Free();
}
