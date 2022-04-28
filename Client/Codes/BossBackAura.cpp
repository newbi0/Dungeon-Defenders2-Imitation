#include "stdafx.h"
#include "Defines.h"
#include "BossBackAura.h"
#include "Renderer.h"
#include "Transform.h"
#include "Rect_Texture.h"
#include "Texture.h"
#include "Shader.h"
#include "Collider.h"
#include "Object_Manager.h"
#include "Unit.h"


CBossBackAura::CBossBackAura(LPDIRECT3DDEVICE9 CBossBackAura)
	: CSkill(CBossBackAura)
{
}

CBossBackAura::CBossBackAura(const CBossBackAura & rhs)
	: CSkill(rhs)
{
}

HRESULT CBossBackAura::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CBossBackAura::Ready_GameObject_Clone(const _vec3 * pPos)
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

	m_pBufferCom->Resize_Buffer(_vec2(-8.f, -8.f), _vec2(8.f, 8.f));

	if (nullptr != pPos)
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, pPos);

	return NOERROR;
}

HRESULT CBossBackAura::Ready_GameObject_Clone(const _matrix & matWorld)
{
	Ready_GameObject_Clone();
	return NOERROR;
}

HRESULT CBossBackAura::Ready_Component(void)
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

	// For.Com_Textrue
	m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_Effect_BossBackAura");
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Texture", m_pTextureCom);

	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Shader_Effect");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	return NOERROR;
}

_int CBossBackAura::Update_GameObject(const _float & fTimeDelta)
{
	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	Compute_ViewZ(&vPos);

	m_fRot += 5.f * fTimeDelta;

	//ºôº¸µå
	_matrix	matView;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);

	ZeroMemory(matView.m[3], sizeof(_vec3));

	D3DXMatrixInverse(&matView, nullptr, &matView);

	D3DXMatrixRotationZ(&matView, m_fRot);
	memcpy(&matView.m[3], &vPos, sizeof(_vec3));

	m_pTransformCom->Set_Infomation(matView);

	return _RUN;
}

_int CBossBackAura::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return _EXIT;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);

	return _RUN;
}

void CBossBackAura::Render_GameObject(void)
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

	_matrix			matView, matProj;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture")))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(1);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();
}

CBossBackAura * CBossBackAura::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBossBackAura* pInstance = new CBossBackAura(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CBossBackAura Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossBackAura::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CBossBackAura(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CBossBackAura Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBossBackAura::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CBossBackAura(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CBossBackAura Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossBackAura::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	CSkill::Free();
}
