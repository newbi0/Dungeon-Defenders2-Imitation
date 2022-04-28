#include "stdafx.h"
#include "Defines.h"
#include "NoCulling_Object.h"

#include "Renderer.h"
#include "Mesh_Static.h"
#include "Shader.h"
#include "Transform.h"
#include "Texture.h"
#include "Object_Manager.h"
#include "Camera.h"

CNoCulling_Object::CNoCulling_Object(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CNoCulling_Object::CNoCulling_Object(const CNoCulling_Object & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNoCulling_Object::Ready_GameObject_Prototype(const _tchar* pMashTag)
{
	m_vHitZonePos = _vec3(109.718811f, -8.80974197f, 89.2799149f);
	m_isHitZone = true;

	if (FAILED(Ready_Component(pMashTag)))
		return E_FAIL;

	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(100.f, 0.f, 100.f));

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	SetUp_LightAndMaterialInfo_OnShader(pEffect);

	return NOERROR;
}

HRESULT CNoCulling_Object::Ready_GameObject_Clone(const _vec3* pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pMeshCom = (Engine::CMesh_Static*)Get_Component(L"Com_Mesh");
	m_pMeshCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	return NOERROR;
}

HRESULT CNoCulling_Object::Ready_GameObject_Clone(const _matrix& matWorld)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pMeshCom = (Engine::CMesh_Static*)Get_Component(L"Com_Mesh");
	m_pMeshCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	m_pTransformCom->Set_Infomation(matWorld);

	return NOERROR;
}

HRESULT CNoCulling_Object::Ready_Component(const _tchar* pMashTag)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	_tchar	szMesh[128] = L"";
	wcscat_s(szMesh, L"Com_Mesh_");
	wcscat_s(szMesh, pMashTag);

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

	// For.Com_Mesh
	m_pMeshCom = (Engine::CMesh_Static*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, szMesh);
	if (nullptr == m_pMeshCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Mesh", m_pMeshCom);
	
	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Shader_Mesh");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	return NOERROR;
}

HRESULT CNoCulling_Object::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

	D3DXMatrixInverse(&matView, nullptr, &matView);
	if (FAILED(pEffect->SetVector("g_vCameraPosition", ((_vec4*)matView.m[3]))))
		return E_FAIL;

	//_vec3	vCamPos;
	//ZeroMemory(vCamPos, sizeof(_vec3));

	//CCamera*	pCamera = dynamic_cast<CCamera*>(CObject_Manager::Get_Instance()->Get_GameObject(SCENE_STAGE, L"Layer_Camera"));
	//if (nullptr == pCamera)
	//	return E_FAIL;
	//vCamPos = pCamera->Get_Camera_Desc()->vEye;

	//if (FAILED(pEffect->SetVector("g_vCameraPosition", &_vec4(vCamPos, 1.f))))
	//	return E_FAIL;

	if (m_eHitZoneType != TYPE_END && true == m_isHitZone)
	{
		pEffect->SetVector("g_vHitZonePos", &_vec4(109.718811f, -8.80974197f, 89.2799149f, 1.f));
		pEffect->SetFloat("g_fHitZoneSize", m_fHitZoneSize);

		m_pHitZomeTextureCom->SetUp_OnShader(pEffect, "g_HitZoneTexture");
		m_pHitZomeTextureCom->SetUp_OnShader(pEffect, "g_HitZoneRoundTexture");
	}

	return NOERROR;
}

HRESULT CNoCulling_Object::SetUp_LightAndMaterialInfo_OnShader(LPD3DXEFFECT pEffect)
{
	//LightInfo
	if (FAILED(pEffect->SetVector("g_vLightDir", &_vec4(1.0f, -1.f, -1.0f, 0.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vLightDiffuse", &_vec4(0.5f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vLightAmvient", &_vec4(0.5f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vLightSpecular", &_vec4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(pEffect->SetVector("g_vMaterialDiffuse", &_vec4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vMaterialAmvient", &_vec4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetVector("g_vMaterialSpecular", &_vec4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(pEffect->SetFloat("g_fSpecularPower", 30.f)))
		return E_FAIL;

	return NOERROR;
}

_int CNoCulling_Object::Update_GameObject(const _float & fTimeDelta)
{
	if (m_eHitZoneType != TYPE_END && true == m_isHitZone)
	{
		m_fHitZoneSize += 5.f * fTimeDelta;
		if (m_fHitZoneSize > 50.f)
		{
			m_eHitZoneType = TYPE_END;
			Safe_Release(m_pHitZomeTextureCom);
			m_isHitZoneFull = true;
		}
	}

	return _RUN;
}

_int CNoCulling_Object::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_PRIORITY, this);

	return _int();
}

void CNoCulling_Object::Render_GameObject(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pShaderCom
		|| nullptr == m_pMeshCom)
		return;

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	if (FAILED(SetUp_ConstantTable(pEffect)))
		return;

	if (FAILED(pEffect->Begin(nullptr, 0)))
		int a = 0;

	if (false == m_isHitZoneFull)
		m_pMeshCom->Render_Mesh(pEffect, 1);
	else
		m_pMeshCom->Render_Mesh(pEffect, 0);

	pEffect->End();	
}

CNoCulling_Object * CNoCulling_Object::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pMashTag)
{
	CNoCulling_Object* pInstance = new CNoCulling_Object(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype(pMashTag)))
	{
		_MSG_BOX("CNoCulling_Object Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNoCulling_Object::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CNoCulling_Object(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CNoCulling_Object Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNoCulling_Object::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CNoCulling_Object(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CNoCulling_Object Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNoCulling_Object::Free(void)
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);

	CGameObject::Free();
}
