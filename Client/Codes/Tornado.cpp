#include "stdafx.h"
#include "Defines.h"
#include "Tornado.h"

#include "Renderer.h"
#include "Mesh_Static.h"
#include "Shader.h"
#include "Transform.h"
#include "Frustum_Culling.h"

CTornado::CTornado(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTornado::CTornado(const CTornado & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTornado::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Scaling(0.03f, 0.03f, 0.03f);

	HANDLE	hFile;

	hFile = CreateFile(L"../Bin/Data/PlayerPos.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	_ulong dwByte = 0;

	for (;;)
	{
		_vec3	pPos;

		ReadFile(hFile, &pPos, sizeof(_vec3), &dwByte, 0);
		if (dwByte == 0)
			break;
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &pPos);
	}

	CloseHandle(hFile);

	return NOERROR;
}

HRESULT CTornado::Ready_GameObject_Clone(const _vec3* pPos)
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

HRESULT CTornado::Ready_GameObject_Clone(const _matrix& matWorld)
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

	m_pTransformCom->Set_Infomation(matWorld);

	return NOERROR;
}

HRESULT CTornado::Ready_Component(void)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

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
	m_pMeshCom = (Engine::CMesh_Static*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Mesh_Tornado");
	if (nullptr == m_pMeshCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Mesh", m_pMeshCom);
	
	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Shader_Effect_Bless");
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

HRESULT CTornado::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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


_int CTornado::Update_GameObject(const _float & fTimeDelta)
{
	return _int();
}

_int CTornado::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_isCulling = m_pCullingCom->isIn_Frustum(m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), 1.f);

	if (true == m_isCulling)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_PRIORITY, this);

	return _int();
}

void CTornado::Render_GameObject(void)
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
		_MSG_BOX("CTornado SetUp_ConstantTable Failed");
		return;
	}

	if (FAILED(pEffect->Begin(nullptr, 0)))
		int a = 0;

	m_pMeshCom->Render_Mesh(pEffect, 0);

	pEffect->End();	
}

CTornado * CTornado::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTornado* pInstance = new CTornado(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CTornado Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTornado::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CTornado(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CTornado Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTornado::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CTornado(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CTornado Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTornado::Free(void)
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pCullingCom);

	CGameObject::Free();
}
