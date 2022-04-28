#include "stdafx.h"
#include "Defines.h"
#include "Cursor.h"
#include "Renderer.h"
#include "Transform.h"
#include "Rect_Texture.h"
#include "Texture.h"
#include "Shader.h"
#include "Collider.h"
#include "Object_Manager.h"
#include "Unit.h"


CCursor::CCursor(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CSkill(pGraphic_Device)
{
}

CCursor::CCursor(const CCursor & rhs)
	: CSkill(rhs)
	, m_isFixity(true)
	, m_isSeeAble(true)
{
}

void CCursor::Set_isFixity(const _bool & isFixity)
{
	m_isFixity = isFixity;

	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(0.f, 0.f, 0.f));
}

HRESULT CCursor::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fViewZ = 0.1f;

	return NOERROR;
}

HRESULT CCursor::Ready_GameObject_Clone(const _vec3 * pPos)
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

	m_pBufferCom->Resize_Buffer(_vec2(-0.1f, -0.1f), _vec2(0.1f, 0.1f));

	if (nullptr != pPos)
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, pPos);

	return NOERROR;
}

HRESULT CCursor::Ready_GameObject_Clone(const _matrix & matWorld)
{
	Ready_GameObject_Clone();
	return NOERROR;
}

HRESULT CCursor::Ready_Component(void)
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
	m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_Cursor");
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

_int CCursor::Update_GameObject(const _float & fTimeDelta)
{
	//_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	//Compute_ViewZ(&vPos);

	m_pTransformCom->RotationZ(fTimeDelta);

	if (true == m_isFixity)
	{
		POINT			ptMouse = { g_iBackCX >> 1, g_iBackCY >> 1 };
		ClientToScreen(g_hWnd, &ptMouse);
		SetCursorPos(ptMouse.x, ptMouse.y);
	}
	else
	{
		_uint	iHalfX = g_iBackCX >> 1;
		_uint	iHalfY = g_iBackCY >> 1;

		POINT			ptCenter = { iHalfX, iHalfY };
		ClientToScreen(g_hWnd, &ptCenter);

		POINT			ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		_vec2		vMouse = _vec2(0.f, 0.f);
		if (iHalfX <= ptMouse.x)
		{
			ptMouse.x -= iHalfX;
			vMouse.x = _float(ptMouse.x) / _float(iHalfX);
		}
		else
		{
			vMouse.x = (1 - (_float(ptMouse.x) / _float(iHalfX))) * -1.f;
		}

		if (iHalfY <= ptMouse.y)
		{
			ptMouse.y -= iHalfY;
			vMouse.y = _float(ptMouse.y) / _float(iHalfY) * -1.f;
		}
		else
		{
			vMouse.y = 1 - (_float(ptMouse.y) / _float(iHalfY));
		}

		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(vMouse.x, vMouse.y, 0.f));
	}

	ShowCursor(false);

	return _RUN;
}

_int CCursor::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return _EXIT;

	if(true == m_isSeeAble)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this);

	return _RUN;
}

void CCursor::Render_GameObject(void)
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
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matProj);
	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	//m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture")))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(3);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();
}

CCursor * CCursor::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCursor* pInstance = new CCursor(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CCursor Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCursor::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CCursor(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CCursor Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCursor::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CCursor(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CCursor Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCursor::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	CSkill::Free();
}
