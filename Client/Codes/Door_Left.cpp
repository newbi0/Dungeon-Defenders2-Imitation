#include "stdafx.h"
#include "Defines.h"
#include "Door_Left.h"

#include "Renderer.h"
#include "Mesh_Static.h"
#include "Shader.h"
#include "Transform.h"
#include "Frustum_Culling.h"
#include "Turn.h"

CDoor_Left::CDoor_Left(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
	, m_pTurn(CTurn::Get_Instance())
{
	m_pTurn->AddRef();
}

CDoor_Left::CDoor_Left(const CDoor_Left & rhs)
	: CGameObject(rhs)
	, m_isOpenning(rhs.m_isOpenning)
	, m_fOpenTime(0)
	, m_pTurn(CTurn::Get_Instance())
{
	m_pTurn->AddRef();
}

HRESULT CDoor_Left::Ready_GameObject_Prototype(const _tchar* pMashTag)
{
	if (FAILED(Ready_Component(pMashTag)))
		return E_FAIL;

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	SetUp_LightAndMaterialInfo_OnShader(pEffect);

	m_fOpenTime = 0;

	return NOERROR;
}

HRESULT CDoor_Left::Ready_GameObject_Clone(const _vec3* pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pMeshCom = (Engine::CMesh_Static*)Get_Component(L"Com_Mesh");
	m_pMeshCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	m_pCullingCom = (Engine::CFrustum_Culling*)Get_Component(L"Com_Culling");
	m_pCullingCom->AddRef();


	return NOERROR;
}

HRESULT CDoor_Left::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	m_pTransformCom->Set_Infomation(matWorld);

	return NOERROR;
}

HRESULT CDoor_Left::Ready_Component(const _tchar* pMashTag)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	_tchar	szMesh[128] = L"";
	wcscat_s(szMesh, L"Com_Mesh_");
	wcscat_s(szMesh, pMashTag);

	// For.Com_Renderer
	m_pRendererCom = (Engine::CRenderer*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Renderer");
	if (nullptr == m_pRendererCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Renderer", m_pRendererCom);

	// For.Com_Transform	
	m_pTransformCom = (Engine::CTransform*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Transform");
	if (nullptr == m_pTransformCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Transform", m_pTransformCom);

	// For.Com_Mesh
	m_pMeshCom = (Engine::CMesh_Static*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, szMesh);
	if (nullptr == m_pMeshCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Mesh", m_pMeshCom);

	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Shader_Mesh");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	// For.Com_Culling
	m_pCullingCom = (Engine::CFrustum_Culling*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Frustum_Culling");
	if (nullptr == m_pCullingCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Culling", m_pCullingCom);

	return NOERROR;
}

HRESULT CDoor_Left::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (FAILED(m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld")))
		return E_FAIL;

	_matrix		matView, matProj;
	if (FAILED(m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView)))
		return E_FAIL;
	if (FAILED(m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj)))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matView", &matView)))
		return E_FAIL;
	if (FAILED(pEffect->SetMatrix("g_matProj", &matProj)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CDoor_Left::SetUp_LightAndMaterialInfo_OnShader(LPD3DXEFFECT pEffect)
{
	//LightInfo
	if (FAILED(pEffect->SetVector("g_vLightDir", &_vec4(1.0f, -5.f, 1.0f, 0.f))))
		return E_FAIL;
 if (FAILED(pEffect->SetVector("g_vLightDiffuse", &_vec4(0.5f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vLightAmvient", &_vec4(0.5f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vLightSpecular", &_vec4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(pEffect->SetVector("g_vMaterialDiffuse", &_vec4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vMaterialAmvient", &_vec4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vMaterialSpecular", &_vec4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetFloat("g_fSpecularPower", 20.f)))
		return E_FAIL;

	return NOERROR;
}

_int CDoor_Left::Update_GameObject(const _float & fTimeDelta)
{
	if (false == m_isOpen && CTurn::TURN_WAVE == m_pTurn->Get_Turn() && 2 == m_pTurn->Get_NextTurn())
	{
		m_isOpen = true;
		m_pTransformCom->RotationY(-3.0f);
	}

	if (true == m_isOpen && CTurn::TURN_BUILD == m_pTurn->Get_Turn() && 2 == m_pTurn->Get_NextTurn())
	{
		m_isOpen = false;
		m_pTransformCom->RotationY(3.0f);
	}

	return _int();
}

_int CDoor_Left::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_isCulling = m_pCullingCom->isIn_Frustum(m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), 2.f);

	if (true == m_isCulling)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_PRIORITY, this);


	return _int();
}

void CDoor_Left::Render_GameObject(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pShaderCom
		|| nullptr == m_pMeshCom)
		return;

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		_MSG_BOX("CDoor_Left SetUp_ConstantTable Failed");
		return;
	}

	if (FAILED(pEffect->Begin(nullptr, 0)))
		int a = 0;

	m_pMeshCom->Render_Mesh(pEffect, 0);

	pEffect->End();
}

CDoor_Left * CDoor_Left::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pMashTag)
{
	CDoor_Left* pInstance = new CDoor_Left(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype(pMashTag)))
	{
		_MSG_BOX("CDoor_Left Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDoor_Left::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CDoor_Left(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CDoor_Left Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDoor_Left::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CDoor_Left(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CDoor_Left Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDoor_Left::Free(void)
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pCullingCom);
	Safe_Release(m_pTurn);

	CGameObject::Free();
}
