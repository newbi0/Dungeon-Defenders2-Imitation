#include "stdafx.h"
#include "Defines.h"
#include "Sign_Check.h"

#include "Renderer.h"
#include "Mesh_Static.h"
#include "Shader.h"
#include "Transform.h"
#include "Texture.h"
#include "Rect_Texture.h"

CSign_Check::CSign_Check(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CSign_Check::CSign_Check(const CSign_Check & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSign_Check::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fViewZ = 0.1f;

	return NOERROR;
}

HRESULT CSign_Check::Ready_GameObject_Clone(const _vec3* pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	m_pBufferCom = (Engine::CRect_Texture*)Get_Component(L"Com_Buffer");
	m_pBufferCom->AddRef();

	m_pTextureCom = (Engine::CTexture*)Get_Component(L"Com_Texture");
	m_pTextureCom->AddRef();


	if (nullptr != pPos)
	{
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, pPos);
	}

	m_pBufferCom->Resize_Buffer(_vec2(-0.05f, -0.07f), _vec2(0.05f, 0.07f));

	return NOERROR;
}

HRESULT CSign_Check::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	return NOERROR;
}

HRESULT CSign_Check::Ready_Component(void)
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
	
	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Shader_UI");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	// For.Com_Buffer
	m_pBufferCom = (Engine::CRect_Texture*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Buffer_RectTexture");
	if (nullptr == m_pBufferCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Buffer", m_pBufferCom);

	// For.Com_Texture
	m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_Sign_Check");
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Texture", m_pTextureCom);

	return NOERROR;
}

HRESULT CSign_Check::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (FAILED(m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld")))
		return E_FAIL;

	_matrix		matTmp;
	D3DXMatrixIdentity(&matTmp);

	if (FAILED(pEffect->SetMatrix("g_matView", &matTmp)))
		return E_FAIL;
	if (FAILED(pEffect->SetMatrix("g_matProj", &matTmp)))
		return E_FAIL;

	return NOERROR;
}

_int CSign_Check::Update_GameObject(const _float & fTimeDelta)
{
	return _RUN;
}

_int CSign_Check::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this);

	return _int();
}

void CSign_Check::Render_GameObject(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pShaderCom
		|| nullptr == m_pBufferCom
		|| nullptr == m_pTextureCom)
		return;

	//ÇÁ·¹ÀÓ
	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		_MSG_BOX("CSign_Check SetUp_ConstantTable Failed");
		return;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture")))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();
}

CSign_Check * CSign_Check::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSign_Check* pInstance = new CSign_Check(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CSign_Check Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSign_Check::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CSign_Check(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CSign_Check Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSign_Check::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CSign_Check(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CSign_Check Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSign_Check::Free(void)
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);

	CGameObject::Free();
}
