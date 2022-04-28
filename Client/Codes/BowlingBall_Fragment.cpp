#include "stdafx.h"
#include "Defines.h"
#include "BowlingBall_Fragment.h"

#include "Renderer.h"
#include "Mesh_Static.h"
#include "Shader.h"
#include "Transform.h"
#include "Frustum_Culling.h"
#include "Collider.h"
#include "Object_Manager.h"

CBowlingBall_Fragment::CBowlingBall_Fragment(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUnit(pGraphic_Device)
{
}

CBowlingBall_Fragment::CBowlingBall_Fragment(const CBowlingBall_Fragment & rhs)
	: CUnit(rhs)
	, m_eType(rhs.m_eType)
	, m_isBoss(rhs.m_isBoss)
{
}

void CBowlingBall_Fragment::Set_Scaling(const _vec3 & vScale)
{
	m_pTransformCom->Scaling(vScale);
}

void CBowlingBall_Fragment::Add_Hp(const _float & fHpAmount)
{
	if (true == m_isAttacked)
	{
		if (fHpAmount < 0)
			m_tUnitInfo.fHp += fHpAmount + m_tUnitInfo.fDef;
		else
			m_tUnitInfo.fHp += fHpAmount;

		if (m_tUnitInfo.fMaxHp < m_tUnitInfo.fHp)
			m_tUnitInfo.fHp = m_tUnitInfo.fMaxHp;

		if (0.f >= m_tUnitInfo.fHp)
		{
			m_pColliderCom->Set_isTargetDead(true);
			m_isDeadAni = true;
		}
	}
}

HRESULT CBowlingBall_Fragment::Ready_GameObject_Prototype(const _tchar * pMashTag, const BALLTYPE & eType, const _bool& isBoss)
{
	Init_UnitInfo(100.f, 0.f, 0.f);
	m_eType = eType;
	m_isBoss = isBoss;

	if (FAILED(Ready_Component(pMashTag)))
		return E_FAIL;

	m_pTransformCom->Scaling(0.01f, 0.01f, 0.01f);

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	SetUp_LightAndMaterialInfo_OnShader(pEffect);

	return NOERROR;
}

HRESULT CBowlingBall_Fragment::Ready_GameObject_Clone(const _vec3* pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pMeshCom = (Engine::CMesh_Static*)Get_Component(L"Com_Mesh");
	m_pMeshCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	m_pCullingCom = (Engine::CFrustum_Culling*)Get_Component(L"Com_Culling");
	m_pCullingCom->AddRef();

	m_pColliderCom = (Engine::CCollider*)Get_Component(L"Com_Collider");
	m_pColliderCom->AddRef();

	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pObject_Manager->AddRef();

	if(nullptr != pPos)
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, pPos);

	_vec3	vMin = *m_pMeshCom->Get_Min();
	_vec3	vMax = *m_pMeshCom->Get_Max();
	vMin.y = -50;

	m_pColliderCom->Set_Collider(CCollider::TYPE_AABB, &vMin, &vMax, m_pTransformCom);

	return NOERROR;
}

HRESULT CBowlingBall_Fragment::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	m_pTransformCom->Set_Infomation(matWorld);

	return NOERROR;
}

HRESULT CBowlingBall_Fragment::Ready_Component(const _tchar* pMashTag)
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

	//For.Com_Collider

	m_pColliderCom = (Engine::CCollider*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Collider");
	if (nullptr == m_pColliderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Collider", m_pColliderCom);

	// For.Com_Culling
	m_pCullingCom = (Engine::CFrustum_Culling*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Frustum_Culling");
	if (nullptr == m_pCullingCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Culling", m_pCullingCom);

	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pObject_Manager->AddRef();

	return NOERROR;
}

HRESULT CBowlingBall_Fragment::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

	return NOERROR;
}

_int CBowlingBall_Fragment::Update_GameObject(const _float & fTimeDelta)
{
	if (true == m_isCulling)
	{
		m_pObject_Manager->Check_Collision_AABB(SCENE_STAGE, m_pColliderCom, L"Layer_Player");
		m_pColliderCom->Update_Collider();

		_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

		if (TYPE_FULL == m_eType)
		{
			if (true == m_isDeadAni)
			{
				(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_BossSkillAura"))->Set_isDead(true);
				m_isDead = true;

				if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_BowlingBall_Fragment_0_Boss", L"Object_BowlingBall_Fragment_0", &vPos)))
					return E_FAIL;
				if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_BowlingBall_Fragment_1_Boss", L"Object_BowlingBall_Fragment_1", &vPos)))
					return E_FAIL;
				if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_BowlingBall_Fragment_2_Boss", L"Object_BowlingBall_Fragment_2", &vPos)))
					return E_FAIL;
				if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_BowlingBall_Fragment_3_Boss", L"Object_BowlingBall_Fragment_3", &vPos)))
					return E_FAIL;
				if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_BowlingBall_Fragment_4_Boss", L"Object_BowlingBall_Fragment_4", &vPos)))
					return E_FAIL;

				CGameObject*	pParts = (m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_BowlingBall_Fragment_0_Boss"));
				((CTransform*)pParts->Get_Component(L"Com_Transform"))->Scaling(0.05f, 0.05f, 0.05f);
				((CCollider*)pParts->Get_Component(L"Com_Collider"))->Set_isTargetDead(true);

				pParts = (m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_BowlingBall_Fragment_1_Boss"));
				((CTransform*)pParts->Get_Component(L"Com_Transform"))->Scaling(0.05f, 0.05f, 0.05f);
				((CCollider*)pParts->Get_Component(L"Com_Collider"))->Set_isTargetDead(true);

				pParts = (m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_BowlingBall_Fragment_2_Boss"));
				((CTransform*)pParts->Get_Component(L"Com_Transform"))->Scaling(0.05f, 0.05f, 0.05f);
				((CCollider*)pParts->Get_Component(L"Com_Collider"))->Set_isTargetDead(true);

				pParts = (m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_BowlingBall_Fragment_3_Boss"));
				((CTransform*)pParts->Get_Component(L"Com_Transform"))->Scaling(0.05f, 0.05f, 0.05f);
				((CCollider*)pParts->Get_Component(L"Com_Collider"))->Set_isTargetDead(true);

				pParts = (m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_BowlingBall_Fragment_4_Boss"));
				((CTransform*)pParts->Get_Component(L"Com_Transform"))->Scaling(0.05f, 0.05f, 0.05f);
				((CCollider*)pParts->Get_Component(L"Com_Collider"))->Set_isTargetDead(true);
			}
		}

		//0 : x, 1 : y, 2 : z, 3 : xy, 4 : yz

		else if (TYPE_FRAGMENT_0 == m_eType)
		{
			_vec3	vDir = _vec3(1.f, -2.f, 0.f);
			D3DXVec3Normalize(&vDir, &vDir);
			m_pTransformCom->Go_Direction(vDir, 5.f * fTimeDelta);
		}

		else if (TYPE_FRAGMENT_1 == m_eType)
		{
			_vec3	vDir = _vec3(0.f, -2.f, 1.f);
			D3DXVec3Normalize(&vDir, &vDir);
			m_pTransformCom->Go_Direction(vDir, 5.f * fTimeDelta);
		}

		else if (TYPE_FRAGMENT_2 == m_eType)
		{
			_vec3	vDir = _vec3(-1.f, -2.f, 0.f);
			D3DXVec3Normalize(&vDir, &vDir);
			m_pTransformCom->Go_Direction(vDir, 5.f * fTimeDelta);
		}

		else if (TYPE_FRAGMENT_3 == m_eType)
		{
			_vec3	vDir = _vec3(0.f, -2.f, -1.f);
			D3DXVec3Normalize(&vDir, &vDir);
			m_pTransformCom->Go_Direction(vDir, 5.f * fTimeDelta);
		}

		else if (TYPE_FRAGMENT_4 == m_eType)
		{
			_vec3	vDir = _vec3(0.5f, -2.f, 0.5f);
			D3DXVec3Normalize(&vDir, &vDir);
			m_pTransformCom->Go_Direction(vDir, 5.f * fTimeDelta);
		}

		if (vPos.y < -9.f)
			Set_isDead(true);
	}

	return _RUN;
}

_int CBowlingBall_Fragment::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_isCulling = m_pCullingCom->isIn_Frustum(m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), 3.f);

	if (true == m_isCulling)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_PRIORITY, this);

	return _RUN;
}

void CBowlingBall_Fragment::Render_GameObject(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pShaderCom
		|| nullptr == m_pMeshCom)
		return;

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		_MSG_BOX("CBowlingBall_Fragment SetUp_ConstantTable Failed");
		return;
	}

	if (FAILED(pEffect->Begin(nullptr, 0)))
		int a = 0;

	m_pMeshCom->Render_Mesh(pEffect, 0);

	pEffect->End();	

	m_pColliderCom->Render_Collider();
}

CBowlingBall_Fragment * CBowlingBall_Fragment::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pMashTag, const BALLTYPE & eType, const _bool& isBoss)
{
	CBowlingBall_Fragment* pInstance = new CBowlingBall_Fragment(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype(pMashTag, eType, isBoss)))
	{
		_MSG_BOX("CBowlingBall_Fragment Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBowlingBall_Fragment::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CBowlingBall_Fragment(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CBowlingBall_Fragment Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBowlingBall_Fragment::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CBowlingBall_Fragment(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CBowlingBall_Fragment Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBowlingBall_Fragment::Free(void)
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pCullingCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pObject_Manager);

	CUnit::Free();
}
