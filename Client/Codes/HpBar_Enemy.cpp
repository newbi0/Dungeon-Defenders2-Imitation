#include "stdafx.h"
#include "Defines.h"
#include "HpBar_Enemy.h"
#include "Renderer.h"
#include "Transform.h"
#include "Rect_Texture.h"
#include "Texture.h"
#include "Shader.h"
#include "Unit.h"


CHpBar_Enemy::CHpBar_Enemy(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CHpBar_Enemy::CHpBar_Enemy(const CHpBar_Enemy & rhs)
	: CUI(rhs)
{
}

HRESULT CHpBar_Enemy::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fViewZ = 0.02f;

	return NOERROR;
}

HRESULT CHpBar_Enemy::Ready_GameObject_Clone(const _vec3 * pPos)
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

	m_pBufferCom->Resize_Buffer(_vec2(-0.83f, 0.f), _vec2(0.83f, 0.1f));

	return NOERROR;
}

HRESULT CHpBar_Enemy::Ready_GameObject_Clone(const _matrix & matWorld)
{
	Ready_GameObject_Clone();
	return NOERROR;
}

HRESULT CHpBar_Enemy::Ready_Component(void)
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

void CHpBar_Enemy::Set_UnitHpBar(CUnit * pUnit)
{
	if (nullptr == pUnit)
		return;

	m_pLinkedUnit = pUnit;
	m_pLinkedUnit->AddRef();
}

_int CHpBar_Enemy::Update_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pLinkedUnit)
		return _RUN;

	UNITINFO	tUnitInfo = *m_pLinkedUnit->Get_UnitInfo();

	_float	fHpRate = (tUnitInfo.fHp / tUnitInfo.fMaxHp) /* * 100.f */;

	_float	fX = 1.66f * fHpRate /* / 100.f */;

	m_fHpX = -0.83f + fX;

	//??????????
	CTransform*	pLinkedTransform = (CTransform*)m_pLinkedUnit->Get_Component(L"Com_Transform");
	if (nullptr != pLinkedTransform)
	{
		_vec3	vLinkedPos = *(pLinkedTransform->Get_Infomation(CTransform::INFO_POSITION));
		vLinkedPos.y += 2.5f;
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &vLinkedPos);
	}

	//??????
	_matrix	matView;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);

	ZeroMemory(matView.m[3], sizeof(_vec3));

	D3DXMatrixInverse(&matView, nullptr, &matView);

	memcpy(&matView.m[3], m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), sizeof(_vec3));

	m_pTransformCom->Set_Infomation(matView);

	return _RUN;
}

_int CHpBar_Enemy::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return _EXIT;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI_PRIORITY, this);

	return _RUN;
}

void CHpBar_Enemy::Render_GameObject(void)
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

	if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", 1)))
		return;

	if (FAILED(pEffect->SetFloat("g_fX", m_fHpX)))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(1);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();	
}

CHpBar_Enemy * CHpBar_Enemy::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHpBar_Enemy* pInstance = new CHpBar_Enemy(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CHpBar_Enemy Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHpBar_Enemy::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CHpBar_Enemy(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CHpBar_Enemy Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHpBar_Enemy::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CHpBar_Enemy(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CHpBar_Enemy Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHpBar_Enemy::Free(void)
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
