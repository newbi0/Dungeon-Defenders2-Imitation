#include "stdafx.h"
#include "Defines.h"
#include "SkillButton.h"

#include "Renderer.h"
#include "Rect_Texture.h"
#include "Texture.h"
#include "Shader.h"
#include "Transform.h"

CSkillButton::CSkillButton(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CSkillButton::CSkillButton(const CSkillButton & rhs)
	: CUI(rhs)
	, m_iIndex(rhs.m_iIndex)
{
}

HRESULT CSkillButton::Ready_GameObject_Prototype(const _uint & iIdx)
{
	m_iIndex = iIdx;
	m_fViewZ = 1.f;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSkillButton::Ready_GameObject_Clone(const _vec3* pPos)
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

	_vec2	vMin, vMax;
	vMin = _vec2(-0.755f, -0.874f);
	vMax = _vec2(-0.66f, -0.722f);

	switch (m_iIndex)
	{
	case 1:
		m_pBufferCom->Resize_Buffer(vMin, vMax);
		break;
	case 2:
		vMin.x += 0.1f;
		vMax.x += 0.1f;
		m_pBufferCom->Resize_Buffer(vMin, vMax);
		break;
	case 3:
		vMin.x += 0.2f;
		vMax.x += 0.2f;
		m_pBufferCom->Resize_Buffer(vMin, vMax);
		break;
	case 4:
		vMin.x += 0.3f;
		vMax.x += 0.3f;
		m_pBufferCom->Resize_Buffer(vMin, vMax);
		break;
	case 5:
		vMin.x += 0.395f;
		vMax.x += 0.395f;
		m_pBufferCom->Resize_Buffer(vMin, vMax);
		break;
	case 6:
		vMin.y += 0.155f;
		vMax.y += 0.155f;
		m_pBufferCom->Resize_Buffer(vMin, vMax);
		break;
	case 7:
		vMin += _vec2(0.1f, 0.155f);
		vMax += _vec2(0.1f, 0.155f);
		m_pBufferCom->Resize_Buffer(vMin, vMax);
		break;
	case 8:
		vMin += _vec2(0.2f, 0.155f);
		vMax += _vec2(0.2f, 0.155f);
		m_pBufferCom->Resize_Buffer(vMin, vMax);
		break;
	case 9:
		vMin += _vec2(0.3f, 0.155f);
		vMax += _vec2(0.3f, 0.155f);
		m_pBufferCom->Resize_Buffer(vMin, vMax);
		break;
	case 0:
		vMin += _vec2(0.395f, 0.155f);
		vMax += _vec2(0.395f, 0.155f);
		m_pBufferCom->Resize_Buffer(vMin, vMax);
		break;
	default:
		m_pBufferCom->Resize_Buffer(vMin, vMax);
		break;
	}

	return NOERROR;
}

HRESULT CSkillButton::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	m_pTransformCom->Set_Infomation(matWorld);

	return NOERROR;
}

HRESULT CSkillButton::Ready_Component(void)
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

	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Shader_UI");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	// For.Com_Texture
	switch (m_iIndex)
	{
	case 0:
		m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_0_slicerTower");
		break;
	case 1:
		m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_1_HealSelf");
		break;
	case 2:
		m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_2_circularSlice");
		break;
	case 3:
		m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_3_bloodRage");
		break;
	case 4:
		m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_4_repair");
		break;
	case 5:
		m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_5_upgrade");
		break;
	case 6:
		m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_6_spikyTower");
		break;
	case 7:
		m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_7_bouncerTower");
		break;
	case 8:
		m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_8_harpoonTower");
		break;
	case 9:
		m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_9_bowlingTower");
		break;
	default:
		break;
	}

	if (nullptr == m_pTextureCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Texture", m_pTextureCom);

	return NOERROR;
}

_int CSkillButton::Update_GameObject(const _float & fTimeDelta)
{
	return _int();
}

_int CSkillButton::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI_PRIORITY, this);

	return _int();
}

void CSkillButton::Render_GameObject(void)
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

CSkillButton * CSkillButton::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _uint& iIdx)
{
 	CSkillButton* pInstance = new CSkillButton(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype(iIdx)))
	{
		_MSG_BOX("CSkillButton Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkillButton::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CSkillButton(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CSkillButton Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkillButton::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CSkillButton(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CSkillButton Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillButton::Free(void)
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);

	CUI::Free();
}

