#include "stdafx.h"
#include "MainApp.h"
#include "Input_Device.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Management.h"
#include "Scene_Logo.h"
#include "Renderer.h"
#include "Rect_Texture.h"
#include "Shader.h"
#include "Transform.h"
#include "Picking.h"
#include "Navigation.h"
#include "KeyChecker.h"
#include "Astar.h"
#include "Sound_Device.h"


CMainApp::CMainApp()
	: m_pObjectMgr(CObject_Manager::Get_Instance())
	, m_pComponentMgr(CComponent_Manager::Get_Instance())
	, m_pManagement(CManagement::Get_Instance())
{
	m_pObjectMgr->AddRef();
	m_pComponentMgr->AddRef();
	m_pManagement->AddRef();
}

HRESULT CMainApp::Ready_MainApp(void)
{
	if (FAILED(Ready_DefaultSetting(CGraphic_Device::MODE_WIN, g_iBackCX, g_iBackCY)))
		return E_FAIL;

	if (FAILED(Ready_StaticComponent()))
		return E_FAIL;

	if (FAILED(Ready_StartScene()))
		return E_FAIL;


	return NOERROR;
}

_int CMainApp::Update_MainApp(const _float & fTimeDelta)
{
	CInput_Device::Get_Instance()->SetUp_Input_State();
	
	m_fTimeAcc += fTimeDelta;

	m_pManagement->Update_CurrentScene(fTimeDelta);

	return _RUN;
}

void CMainApp::Render_MainApp(void)
{
	if (nullptr == m_pGraphic_Device)
		return;

	++m_dwRenderCnt;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, L"FPS:%d", m_dwRenderCnt);
		m_dwRenderCnt = 0;
		m_fTimeAcc = 0.f;
	}

	SetWindowText(g_hWnd, m_szFPS);

	m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.0f, 0.f, 1.f, 1.f), 1.f, 0);

	m_pGraphic_Device->BeginScene();

	m_pManagement->Render_CurrentScene();

	m_pGraphic_Device->EndScene();

	m_pGraphic_Device->Present(nullptr, nullptr, 0, nullptr);
}

HRESULT CMainApp::Ready_DefaultSetting(CGraphic_Device::WINMODE eMode, const _uint & iBackSizeX, const _uint & iBackSizeY)
{
	if (FAILED(CGraphic_Device::Get_Instance()->Ready_GraphicDev(g_hWnd, eMode, g_iBackCX, g_iBackCY, &m_pGraphic_Device)))
		return E_FAIL;

	if (FAILED(CInput_Device::Get_Instance()->Ready_Input_Device(g_hInst, g_hWnd)))
		return E_FAIL;

	if (FAILED(CSound_Device::Get_Instance()->Init(g_hWnd)))
		return E_FAIL;
	
	if (nullptr == m_pComponentMgr
		|| nullptr == m_pObjectMgr)
		return E_FAIL;

	if (FAILED(m_pComponentMgr->Reserve_Container_Size(SCENE_END)))
		return E_FAIL;

	if (FAILED(m_pObjectMgr->Reserve_Container_Size(SCENE_END)))
		return E_FAIL;
	
	srand((unsigned)time(NULL));

	return NOERROR;
}

HRESULT CMainApp::Ready_StaticComponent(void)
{
	CComponent*		pComponent = nullptr;

	pComponent = CRenderer::Create(m_pGraphic_Device);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Renderer", pComponent)))
		return E_FAIL;

	pComponent = CTransform::Create(m_pGraphic_Device);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Transform", pComponent)))
		return E_FAIL;

	pComponent = CRect_Texture::Create(m_pGraphic_Device);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Buffer_RectTexture", pComponent)))
		return E_FAIL;

	pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_Texture.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Shader_Texture", pComponent)))
		return E_FAIL;

	pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_UI.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Shader_UI", pComponent)))
		return E_FAIL;

	pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_Color.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Shader_Color", pComponent)))
		return E_FAIL;

	pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_Effect.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Shader_Effect", pComponent)))
		return E_FAIL;

	pComponent = CPicking::Create(m_pGraphic_Device);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Picking", pComponent)))
		return E_FAIL;

	CNavigation*	pNavigation = CNavigation::Create(m_pGraphic_Device, SCENE_STATIC, L"Com_Picking", L"Com_Shader_Color");
	if (nullptr == pNavigation)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Navigation", pNavigation)))
		return E_FAIL;

	pComponent = CAStar::Create(m_pGraphic_Device, pNavigation);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_AStar", pComponent)))
		return E_FAIL;

	RECT	rcWindow;
	GetWindowRect(g_hWnd, &rcWindow);

	pComponent = CKeyChecker::Create(m_pGraphic_Device, rcWindow);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_KeyChecker", pComponent)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMainApp::Ready_StartScene(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	CRenderer*	pRenderer = (CRenderer*)m_pComponentMgr->Clone_Component(SCENE_STATIC, L"Com_Renderer");
	if (nullptr == pRenderer)
		return E_FAIL;

	if (nullptr == m_pManagement)
		return E_FAIL;

	CScene*		pScene = CScene_Logo::Create(m_pGraphic_Device, pRenderer);
	if (nullptr == pScene)
		return E_FAIL;
	if (FAILED(m_pManagement->SetUp_CurrentScene(pScene)))
		return E_FAIL;

	Safe_Release(pRenderer);

	return NOERROR;
}

CMainApp * CMainApp::Create(void)
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Ready_MainApp()))
	{
		_MSG_BOX("CMainApp Created Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free(void)
{
	Safe_Release(m_pObjectMgr);
	Safe_Release(m_pComponentMgr);
	Safe_Release(m_pManagement);

	Safe_Release(m_pGraphic_Device);
}
