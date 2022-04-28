#include "stdafx.h"
#include "Defines.h"
#include "SkillBar_Boss.h"
#include "Renderer.h"
#include "Transform.h"
#include "Rect_Texture.h"
#include "Texture.h"
#include "Shader.h"
#include "Unit.h"


CSkillBar_Boss::CSkillBar_Boss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CSkillBar_Boss::CSkillBar_Boss(const CSkillBar_Boss & rhs)
	: CUI(rhs)
{
}

HRESULT CSkillBar_Boss::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fViewZ = 0.2f;

	return NOERROR;
}

HRESULT CSkillBar_Boss::Ready_GameObject_Clone(const _vec3 * pPos)
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

	m_pBufferCom->Resize_Buffer(_vec2(0.f, 0.f), _vec2(0.6f, 0.04f));
	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(-0.1f, 0.75f, 0.f));

	return NOERROR;
}

HRESULT CSkillBar_Boss::Ready_GameObject_Clone(const _matrix & matWorld)
{
	Ready_GameObject_Clone();
	return NOERROR;
}

HRESULT CSkillBar_Boss::Ready_Component(void)
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
	m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_HpBar_Enemy");
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Texture", m_pTextureCom);

	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Shader_HpBar");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	return NOERROR;
}

void CSkillBar_Boss::Set_UnitHpBar(CUnit * pUnit)
{
	if (nullptr == pUnit)
		return;

	m_pLinkedUnit = pUnit;
	m_pLinkedUnit->AddRef();
}

_int CSkillBar_Boss::Update_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pLinkedUnit)
		return _RUN;

	_float	fGage = *m_pLinkedUnit->Get_SkillGage();

	_float	fX = 0.6f * fGage;

	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

	m_fGageX = -0.1f + fX;

	return _RUN;
}

_int CSkillBar_Boss::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return _EXIT;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this);

	return _RUN;
}

void CSkillBar_Boss::Render_GameObject(void)
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

	if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", 3)))
		return;

	if (FAILED(pEffect->SetFloat("g_fX", m_fGageX)))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(2);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();
}

CSkillBar_Boss * CSkillBar_Boss::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSkillBar_Boss* pInstance = new CSkillBar_Boss(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CSkillBar_Boss Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkillBar_Boss::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CSkillBar_Boss(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CSkillBar_Boss Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkillBar_Boss::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CSkillBar_Boss(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CSkillBar_Boss Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillBar_Boss::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	if (nullptr != m_pLinkedUnit)
		Safe_Release(m_pLinkedUnit);

	CUI::Free();
}
