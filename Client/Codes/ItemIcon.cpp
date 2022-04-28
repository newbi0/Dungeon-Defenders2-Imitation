#include "stdafx.h"
#include "Defines.h"
#include "ItemIcon.h"

#include "Renderer.h"
#include "Mesh_Static.h"
#include "Shader.h"
#include "Transform.h"
#include "Texture.h"
#include "Rect_Texture.h"

CItemIcon::CItemIcon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CItemIcon::CItemIcon(const CItemIcon & rhs)
	: CGameObject(rhs)
{
}

void CItemIcon::Set_ItemInfo(const _tchar * pMeshTag, const ITEMINFO * pInfo)
{
	m_pMeshCom = (CMesh_Static*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, pMeshTag);
	if (nullptr == m_pMeshCom)
		return;
	m_pMeshCom->AddRef();

	if (nullptr == pInfo)
		return;

	memcpy(&m_tUnitInfo, pInfo, sizeof(ITEMINFO));
}

HRESULT CItemIcon::Ready_GameObject_Prototype()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);

	return NOERROR;
}

HRESULT CItemIcon::Ready_GameObject_Clone(const _vec3* pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pGraphicTransformCom = (Engine::CTransform*)Get_Component(L"Com_GraphicTransform");
	m_pGraphicTransformCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	m_pBufferCom = (Engine::CRect_Texture*)Get_Component(L"Com_Buffer");
	m_pBufferCom->AddRef();

	m_pTextureCom = (Engine::CTexture*)Get_Component(L"Com_Texture");
	m_pTextureCom->AddRef();

	m_pGraphicShaderCom = (Engine::CShader*)Get_Component(L"Com_GraphicShader");
	m_pGraphicShaderCom->AddRef();

	if (nullptr != pPos)
	{
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, pPos);
		m_pGraphicTransformCom->Set_Infomation(CTransform::INFO_POSITION, pPos);
	}

	m_pBufferCom->Resize_Buffer(_vec3(-0.5f, 0.f, -0.5f), _vec3(0.5f, 0.f, 0.5f));

	return NOERROR;
}

HRESULT CItemIcon::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	return NOERROR;
}

HRESULT CItemIcon::Ready_Component(void)
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

	// For.Com_GraphicTransform	
	m_pGraphicTransformCom = (Engine::CTransform*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Transform");
	if (nullptr == m_pGraphicTransformCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_GraphicTransform", m_pGraphicTransformCom);
	
	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Shader_Mesh");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	// For.Com_Buffer
	m_pBufferCom = (Engine::CRect_Texture*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Buffer_RectTexture");
	if (nullptr == m_pBufferCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Buffer", m_pBufferCom);

	// For.Com_Texture
	m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_PickupGraphic");
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Texture", m_pTextureCom);

	// For.Com_GraphicShader
	m_pGraphicShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Shader_Effect");
	if (nullptr == m_pGraphicShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_GraphicShader", m_pGraphicShaderCom);

	return NOERROR;
}

HRESULT CItemIcon::SetUp_ConstantTable(CTransform* pTransform, LPD3DXEFFECT pEffect)
{
	if (FAILED(pTransform->SetUp_OnShader(pEffect, "g_matWorld")))
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

_int CItemIcon::Update_GameObject(const _float & fTimeDelta)
{
	m_pTransformCom->RotationY(1.f * fTimeDelta);
	m_pGraphicTransformCom->RotationY(1.f, fTimeDelta);

	return _RUN;
}

_int CItemIcon::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_PRIORITY, this);

	return _int();
}

void CItemIcon::Render_GameObject(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pGraphicTransformCom
		|| nullptr == m_pShaderCom
		|| nullptr == m_pGraphicShaderCom
		|| nullptr == m_pBufferCom
		|| nullptr == m_pTextureCom
		|| nullptr == m_pMeshCom)
		return;

	//아이콘 매쉬
	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	if (FAILED(SetUp_ConstantTable(m_pTransformCom, pEffect)))
	{
		_MSG_BOX("CItemIcon SetUp_ConstantTable Failed");
		return;
	}

	if (FAILED(pEffect->Begin(nullptr, 0)))
		int a = 0;

	m_pMeshCom->Render_Mesh(pEffect, 0);

	pEffect->End();

	//그래픽
	pEffect = m_pGraphicShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	if (FAILED(m_pGraphicTransformCom->SetUp_OnShader(pEffect, "g_matWorld")))
		return;

	_matrix			matView, matProj;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture")))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(6);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();
}

CItemIcon * CItemIcon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CItemIcon* pInstance = new CItemIcon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CItemIcon Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CItemIcon::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CItemIcon(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CItemIcon Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CItemIcon::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CItemIcon(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CItemIcon Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItemIcon::Free(void)
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pGraphicTransformCom);
	if(nullptr != m_pMeshCom)
		Safe_Release(m_pMeshCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	CGameObject::Free();
}
