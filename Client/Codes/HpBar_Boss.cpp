#include "stdafx.h"
#include "Defines.h"
#include "HpBar_Boss.h"
#include "Renderer.h"
#include "Transform.h"
#include "Rect_Texture.h"
#include "Texture.h"
#include "Shader.h"
#include "Unit.h"


CHpBar_Boss::CHpBar_Boss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CHpBar_Boss::CHpBar_Boss(const CHpBar_Boss & rhs)
	: CUI(rhs)
{
}

HRESULT CHpBar_Boss::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fViewZ = 0.2f;

	return NOERROR;
}

HRESULT CHpBar_Boss::Ready_GameObject_Clone(const _vec3 * pPos)
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

	m_pBufferCom->Resize_Buffer(_vec2(-0.5f, 0.f), _vec2(0.5f, 0.05f));
	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(0.f, 0.85f, 0.f));

	return NOERROR;
}

HRESULT CHpBar_Boss::Ready_GameObject_Clone(const _matrix & matWorld)
{
	Ready_GameObject_Clone();
	return NOERROR;
}

HRESULT CHpBar_Boss::Ready_Component(void)
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
	m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_HpBar_Boss");
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

void CHpBar_Boss::Set_UnitHpBar(CUnit * pUnit)
{
	if (nullptr == pUnit)
		return;

	m_pLinkedUnit = pUnit;
	m_pLinkedUnit->AddRef();
}

_int CHpBar_Boss::Update_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pLinkedUnit)
		return _RUN;

	UNITINFO	tUnitInfo = *m_pLinkedUnit->Get_UnitInfo();

	_float	fHpRate = (tUnitInfo.fHp / tUnitInfo.fMaxHp) /* * 100.f */;

	_float	fX = 1.f * fHpRate /* / 100.f */;

	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

	m_fHpX = -0.5f + fX;

	return _RUN;
}

_int CHpBar_Boss::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return _EXIT;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this);

	return _RUN;
}

void CHpBar_Boss::Render_GameObject(void)
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

	if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", 2)))
		return;

	if (FAILED(pEffect->SetFloat("g_fX", m_fHpX)))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(2);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();	
}

CHpBar_Boss * CHpBar_Boss::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHpBar_Boss* pInstance = new CHpBar_Boss(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CHpBar_Boss Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHpBar_Boss::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CHpBar_Boss(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CHpBar_Boss Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHpBar_Boss::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CHpBar_Boss(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CHpBar_Boss Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHpBar_Boss::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	if(nullptr != m_pLinkedUnit)
		Safe_Release(m_pLinkedUnit);

	CUI::Free();
}
