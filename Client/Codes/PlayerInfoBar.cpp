#include "stdafx.h"
#include "Defines.h"
#include "PlayerInfoBar.h"

#include "Object_Manager.h"

#include "Renderer.h"
#include "Rect_Texture.h"
#include "Texture.h"
#include "Shader.h"
#include "Transform.h"

#include "SkillButton.h"

CPlayerInfoBar::CPlayerInfoBar(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CPlayerInfoBar::CPlayerInfoBar(const CPlayerInfoBar & rhs)
	: CUI(rhs)
	, m_vecSkillButton(rhs.m_vecSkillButton)
{
}

HRESULT CPlayerInfoBar::Ready_GameObject_Prototype(void)
{
	m_fViewZ = 0.f;

	if (FAILED(Ready_Component())
		|| FAILED(Ready_SkillButton()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPlayerInfoBar::Ready_GameObject_Clone(const _vec3* pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pBufferCom = (Engine::CRect_Texture*)Get_Component(L"Com_Buffer");
	m_pBufferCom->AddRef();

	m_pTextureCom = (Engine::CTexture*)Get_Component(L"Com_Texture");
	m_pTextureCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	_vec2	vMin = _vec2(-1.f, -1.f);
	_vec2	vMax = _vec2(-0.25f, -0.4f);
	m_pBufferCom->Resize_Buffer(vMin, vMax);

	return NOERROR;
}

HRESULT CPlayerInfoBar::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	m_pTransformCom->Set_Infomation(matWorld);

	return NOERROR;
}

HRESULT CPlayerInfoBar::Ready_Component(void)
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

	// For.Com_Buffer	
	m_pBufferCom = (Engine::CRect_Texture*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Buffer_RectTexture");
	if (nullptr == m_pBufferCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Buffer", m_pBufferCom);

	// For.Com_Texture	
	m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_PlayerInfoBar");
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Texture", m_pTextureCom);

	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Shader_UI");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	return NOERROR;
}

HRESULT CPlayerInfoBar::Ready_SkillButton(void)
{

	return NOERROR;
}

_int CPlayerInfoBar::Update_GameObject(const _float & fTimeDelta)
{
	return _int();
}

_int CPlayerInfoBar::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI_PRIORITY, this);

	return _int();
}

void CPlayerInfoBar::Render_GameObject(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom
		|| nullptr == m_pShaderCom
		|| nullptr == m_pBufferCom)
		return;

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	if (FAILED(m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld")))
		return;

	_matrix			matTmp;
	D3DXMatrixIdentity(&matTmp);

	pEffect->SetMatrix("g_matView", &matTmp);
	pEffect->SetMatrix("g_matProj", &matTmp);

	if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture")))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();	
}

CPlayerInfoBar * CPlayerInfoBar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
 	CPlayerInfoBar* pInstance = new CPlayerInfoBar(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CPlayerInfoBar Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerInfoBar::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CPlayerInfoBar(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CPlayerInfoBar Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayerInfoBar::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CPlayerInfoBar(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CPlayerInfoBar Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerInfoBar::Free(void)
{
	if (false == m_isClone)
	{
		for (auto iter : m_vecSkillButton)
			Safe_Release(iter);
		m_vecSkillButton.clear();
	}

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);

	CUI::Free();
}

