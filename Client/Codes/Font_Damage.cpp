#include "stdafx.h"
#include "Defines.h"
#include "Font_Damage.h"
#include "Renderer.h"
#include "Transform.h"
#include "Rect_Texture.h"
#include "Texture.h"
#include "Shader.h"


CFont_Damage::CFont_Damage(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CFont_Damage::CFont_Damage(const CFont_Damage & rhs)
	: CUI(rhs)
{
}

void CFont_Damage::Set_Font(const _uint & iDamage)
{
	_uint	iDamage2 = iDamage / 100;
	if (0 != iDamage2)
		m_iDamege_2 = iDamage2;
	else
		m_iDamege_2 = 0;

	_uint	iDamage1 = iDamage - (iDamage2 * 100.f);
	iDamage1 /= 10;
	if (0 != iDamage1)
		m_iDamege_1 = iDamage1;
	else
		m_iDamege_1 = 0;

	_uint	iDamage0 = iDamage - (iDamage2 * 100.f) - (iDamage1 * 10.f);
	if (0 != iDamage0)
		m_iDamege_0 = iDamage0;
	else
		m_iDamege_0 = 0;
}

HRESULT CFont_Damage::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CFont_Damage::Ready_GameObject_Clone(const _vec3 * pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pBufferCom_0 = (Engine::CRect_Texture*)Get_Component(L"Com_Buffer_0");
	m_pBufferCom_0->AddRef();

	m_pBufferCom_1 = (Engine::CRect_Texture*)Get_Component(L"Com_Buffer_1");
	m_pBufferCom_1->AddRef();

	m_pBufferCom_2 = (Engine::CRect_Texture*)Get_Component(L"Com_Buffer_2");
	m_pBufferCom_2->AddRef();

	m_pTextureCom = (Engine::CTexture*)Get_Component(L"Com_Texture");
	m_pTextureCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	//m_pBufferCom_0->Resize_Buffer(_vec2(-0.3f, -0.3f), _vec2(0.3f, 0.3f));
	//m_pBufferCom_1->Resize_Buffer(_vec2(-0.3f, -0.3f), _vec2(0.3f, 0.3f));
	//m_pBufferCom_2->Resize_Buffer(_vec2(-0.3f, -0.3f), _vec2(0.3f, 0.3f));

	m_pBufferCom_2->Resize_Buffer(_vec2(-0.6f, -0.2f), _vec2(-0.1f, 0.3f));
	m_pBufferCom_1->Resize_Buffer(_vec2(-0.2f, -0.2f), _vec2(0.3f, 0.3f));
	m_pBufferCom_0->Resize_Buffer(_vec2(0.2f, -0.2f), _vec2(0.7f, 0.3f));


	if (nullptr != pPos)
	{
		m_vWholePos = *pPos;
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &m_vWholePos);
	}

	return NOERROR;
}

HRESULT CFont_Damage::Ready_GameObject_Clone(const _matrix & matWorld)
{
	Ready_GameObject_Clone();
	return NOERROR;
}

HRESULT CFont_Damage::Ready_Component(void)
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
	m_pBufferCom_0 = (Engine::CRect_Texture*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Buffer_RectTexture");
	if (nullptr == m_pBufferCom_0)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Buffer_0", m_pBufferCom_0);

	// For.Com_Buffer
	m_pBufferCom_1 = (Engine::CRect_Texture*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Buffer_RectTexture");
	if (nullptr == m_pBufferCom_1)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Buffer_1", m_pBufferCom_1);

	// For.Com_Buffer
	m_pBufferCom_2 = (Engine::CRect_Texture*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Buffer_RectTexture");
	if (nullptr == m_pBufferCom_2)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Buffer_2", m_pBufferCom_2);

	// For.Com_Textrue
	m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_Font_Damage");
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

_int CFont_Damage::Update_GameObject(const _float & fTimeDelta)
{
	_vec3 vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

	m_pTransformCom->Go_Direction(_vec3(0.f, 1.f, 0.f), 3.f * fTimeDelta);
	
	if (2.f < vPos.y - m_vWholePos.y)
		Set_isDead(true);

	//빌보드
	_matrix	matView;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);

	ZeroMemory(matView.m[3], sizeof(_vec3));

	D3DXMatrixInverse(&matView, nullptr, &matView);

	memcpy(&matView.m[3], m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), sizeof(_vec3));

	m_pTransformCom->Set_Infomation(matView);

	//Z
	Compute_ViewZ(&vPos);

	return _RUN;
}

_int CFont_Damage::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return _EXIT;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this);

	return _RUN;
}

void CFont_Damage::Render_GameObject(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom
		|| nullptr == m_pShaderCom
		|| nullptr == m_pBufferCom_0)
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

	//첫번째 자리
	if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", m_iDamege_0)))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(2);

	m_pBufferCom_0->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();


	//두번째 자리
	if (0 != m_iDamege_1)
	{
		if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", m_iDamege_1)))
			return;

		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(2);

		m_pBufferCom_1->Render_VIBuffer();

		pEffect->EndPass();
		pEffect->End();
	}

	//세번째 자리
	if (0 != m_iDamege_2)
	{
		if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", m_iDamege_2)))
			return;

		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(2);

		m_pBufferCom_2->Render_VIBuffer();

		pEffect->EndPass();
		pEffect->End();
	}
}

CFont_Damage * CFont_Damage::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFont_Damage* pInstance = new CFont_Damage(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CFont_Damage Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFont_Damage::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CFont_Damage(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CFont_Damage Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFont_Damage::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CFont_Damage(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CFont_Damage Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFont_Damage::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom_0);
	Safe_Release(m_pBufferCom_1);
	Safe_Release(m_pBufferCom_2);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
