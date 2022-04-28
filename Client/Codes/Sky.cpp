#include "stdafx.h"
#include "Sky.h"
#include "Defines.h"
#include "Renderer.h"
#include "Transform.h"
#include "Cube_Texture.h"
#include "Texture.h"
#include "Shader.h"
#include "Object_Manager.h"
#include "Camera.h"

CSky::CSky(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CSky::CSky(const CSky & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSky::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	return NOERROR;
}

HRESULT CSky::Ready_GameObject_Clone(const _vec3 * pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pBufferCom = (Engine::CCube_Texture*)Get_Component(L"Com_Buffer");
	m_pBufferCom->AddRef();
	
	m_pTextureCom = (Engine::CTexture*)Get_Component(L"Com_Texture");
	m_pTextureCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	_matrix			matView;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixInverse(&matView, nullptr, &matView);

	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, matView.m[3][0], matView.m[3][1], matView.m[3][2]);

	return NOERROR;
}

HRESULT CSky::Ready_GameObject_Clone(const _matrix & matWorld)
{
	Ready_GameObject_Clone();

	return NOERROR;
}

HRESULT CSky::Ready_Component(void)
{
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
	m_pBufferCom = (Engine::CCube_Texture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Cube_Texture");
	if (nullptr == m_pBufferCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Buffer", m_pBufferCom);

	// For.Com_Mesh
	m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_Sky");
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Texture", m_pTextureCom);

	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Shader_Sky");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	return NOERROR;
}

HRESULT CSky::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (FAILED(m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld")))
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

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture");

	return NOERROR;
}

_int CSky::Update_GameObject(const _float & fTimeDelta)
{
	//_matrix			matView;
	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);

	//D3DXMatrixInverse(&matView, nullptr, &matView);

	//m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, matView.m[3][0], matView.m[3][1], matView.m[3][2]);

	//_vec3	vCameraPos;
	//memcpy(&vCameraPos, &matView.m[3][0], sizeof(_vec3));
	//
	//_vec3	vDir = vCameraPos - *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	//if(D3DXVec3Length(&vDir) > 0.1f)
	//	m_pTransformCom->Go_Direction(vDir, fTimeDelta * 5.f);

	return _RUN;
}

_int CSky::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CCamera*	pCamera = (CCamera*)CObject_Manager::Get_Instance()->Get_GameObject(SCENE_STAGE, L"Layer_Camera");
	if (pCamera == nullptr)
		return E_FAIL;
	CAMERA_DESC CameraDesc = *(pCamera->Get_Camera_Desc());
	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &CameraDesc.vEye);

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_SKY, this);

	return _RUN;
}

void CSky::Render_GameObject(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pShaderCom
		|| nullptr == m_pBufferCom)
		return;

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		_MSG_BOX("CSky SetUp_ConstantTable Failed");
		return;
	}

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();
}

CSky * CSky::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSky*	pInstance = new CSky(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CSky Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSky::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CSky(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CSky Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSky::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CSky(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CSky Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSky::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	CGameObject::Free();
}
