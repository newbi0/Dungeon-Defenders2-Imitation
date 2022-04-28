#include "stdafx.h"
#include "Defines.h"
#include "MainCrystal.h"

#include "Renderer.h"
#include "Mesh_Static.h"
#include "Shader.h"
#include "Transform.h"
#include "Frustum_Culling.h"
#include "Collider.h"
#include "Object_Manager.h"

CMainCrystal::CMainCrystal(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUnit(pGraphic_Device)
{
}

CMainCrystal::CMainCrystal(const CMainCrystal & rhs)
	: CUnit(rhs)
{
}

HRESULT CMainCrystal::Ready_GameObject_Prototype(const _tchar* pMashTag)
{
	Init_UnitInfo(10000.f, 0.f, 50.f);

	if (FAILED(Ready_Component(pMashTag)))
		return E_FAIL;

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	SetUp_LightAndMaterialInfo_OnShader(pEffect);

	return NOERROR;
}

HRESULT CMainCrystal::Ready_GameObject_Clone(const _vec3* pPos)
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

	m_pColliderCom = (Engine::CCollider*)Get_Component(L"Com_Collider");
	m_pColliderCom->AddRef();

	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pObject_Manager->AddRef();

	return NOERROR;
}

HRESULT CMainCrystal::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	m_pTransformCom->Set_Infomation(matWorld);
	m_pTransformCom->Scaling(0.015f, 0.015f, 0.015f);

	_vec3	vMin = *m_pMeshCom->Get_Min();
	_vec3	vMax = *m_pMeshCom->Get_Max();

	m_pColliderCom->Set_Collider(CCollider::TYPE_AABB, &vMin, &vMax, m_pTransformCom);

	return NOERROR;
}

HRESULT CMainCrystal::Ready_Component(const _tchar* pMashTag)
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
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Shader_Light");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	// For.Com_Culling
	m_pCullingCom = (Engine::CFrustum_Culling*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Frustum_Culling");
	if (nullptr == m_pCullingCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Culling", m_pCullingCom);

	// For.Com_Collider
	m_pColliderCom = (Engine::CCollider*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Collider");
	if (nullptr == m_pColliderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Collider", m_pColliderCom);

	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pObject_Manager->AddRef();

	return NOERROR;
}

HRESULT CMainCrystal::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

_int CMainCrystal::Update_GameObject(const _float & fTimeDelta)
{
	if (true == m_isCulling)
	{
		m_pTransformCom->RotationY(1.f, fTimeDelta);

		m_pObject_Manager->Check_Collision_AABB(SCENE_STAGE, m_pColliderCom, L"Layer_Player");

		m_pColliderCom->Update_Collider();
	}

	return _int();
}

_int CMainCrystal::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_isCulling = m_pCullingCom->isIn_Frustum(m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), 3.f);

	if (true == m_isCulling)
	{
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);
		Compute_ViewZ(m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION));
	}


	return _int();
}

void CMainCrystal::Render_GameObject(void)
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
		_MSG_BOX("CMainCrystal SetUp_ConstantTable Failed");
		return;
	}

	if (FAILED(pEffect->Begin(nullptr, 0)))
		int a = 0;

	m_pMeshCom->Render_Mesh(pEffect, 1);

	pEffect->End();	

	m_pColliderCom->Render_Collider();
}

CMainCrystal * CMainCrystal::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pMashTag)
{
	CMainCrystal* pInstance = new CMainCrystal(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype(pMashTag)))
	{
		_MSG_BOX("CMainCrystal Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMainCrystal::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CMainCrystal(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CMainCrystal Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMainCrystal::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CMainCrystal(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CMainCrystal Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainCrystal::Free(void)
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pCullingCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pObject_Manager);

	CUnit::Free();
}
