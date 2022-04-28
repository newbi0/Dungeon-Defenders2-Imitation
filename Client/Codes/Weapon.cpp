#include "stdafx.h"
#include "Defines.h"
#include "Weapon.h"

#include "Renderer.h"
#include "Mesh_Static.h"
#include "Mesh_Dynamic.h"
#include "Shader.h"
#include "Transform.h"
#include "Navigation.h"
#include "Collider.h"

#include "Input_Device.h"
#include "Object_Manager.h"
#include "Turn.h"
#include "Unit.h"
#include "Player.h"
#include "Goblin.h"

CWeapon::CWeapon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CGameObject(rhs)
	, m_matParent(rhs.m_matParent)
	, m_pHandMatrix(rhs.m_pHandMatrix)
	, m_vMin(rhs.m_vMin)
	, m_vMax(rhs.m_vMax)
	, m_ColliderMatrix(rhs.m_ColliderMatrix)
	, m_isAtt(false)
{
}

const _matrix * CWeapon::Get_WorldMatrix(void) const
{
	if (nullptr == m_pTransformCom)
	{
		_matrix	matIden;
		return D3DXMatrixIdentity(&matIden);
	}
	return m_pTransformCom->Get_WorldMatrix();
}


HRESULT CWeapon::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Scaling(0.2f, 0.2f, 0.2f);

	m_pTransformCom->RotationZ(180.f);
	m_pTransformCom->RotationY(180.f);

	//Min, Max
	m_vMin = *m_pMeshCom->Get_Min();
	m_vMax = *m_pMeshCom->Get_Max();

	m_vMin *= 0.2f;
	m_vMax *= 0.2f;

	return NOERROR;
}

HRESULT CWeapon::Ready_GameObject_Clone(const _vec3* pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pMeshCom = (Engine::CMesh_Static*)Get_Component(L"Com_Mesh");
	m_pMeshCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	m_pColliderCom = (Engine::CCollider*)Get_Component(L"Com_Collider");
	m_pColliderCom->AddRef();

	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pObject_Manager->AddRef();

	m_pReleventUnit = (CUnit*)m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player");
	if (nullptr == m_pReleventUnit)
		return E_FAIL;
	m_pReleventUnit->AddRef();

	((CPlayer*)m_pReleventUnit)->Set_Weapon(this);

	//부모행렬 기초 세팅
	m_pRelevent_TransformCom = (CTransform*)m_pObject_Manager->Get_Component(SCENE_STAGE, L"Layer_Player", L"Com_Transform");
	if (nullptr == m_pRelevent_TransformCom)
		return E_FAIL;
	m_pRelevent_TransformCom->AddRef();

	CMesh_Dynamic*	pPlayerMesh = (CMesh_Dynamic*)m_pObject_Manager->Get_Component(SCENE_STAGE, L"Layer_Player", L"Com_Mesh");
	if (nullptr == pPlayerMesh)
		return E_FAIL;
	m_pHandMatrix = pPlayerMesh->Get_FrameCombinedMatrix("b_HandR");

	D3DXMatrixIdentity(&m_matParent);

	m_pTransformCom->Set_ParentMatrixPointer(&m_matParent);

	m_pColliderCom->Set_Collider(CCollider::TYPE_OBB, &m_vMin, &m_vMax, m_pTransformCom);

	return NOERROR;
}

HRESULT CWeapon::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	return NOERROR;
}

HRESULT CWeapon::Ready_Component(void)
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

	// For.Com_Mesh
	m_pMeshCom = (Engine::CMesh_Static*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Mesh_Sword01");
	if (nullptr == m_pMeshCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Mesh", m_pMeshCom);
	
	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Shader_Mesh");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	// For.Com_Collider
	m_pColliderCom = (Engine::CCollider*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Collider");
	if (nullptr == m_pColliderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Collider", m_pColliderCom);

	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pObject_Manager->AddRef();

	return NOERROR;
}

HRESULT CWeapon::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

void CWeapon::Update_Collider(void)
{
	_vec3	vCenter = *(_vec3*)m_matParent.m[3];

	_matrix	matWorld, matY, matZ;
	D3DXMatrixIdentity(&matWorld);

	D3DXMatrixRotationY(&matY, 119.9f);
	D3DXMatrixRotationZ(&matZ, -45.f);

	matWorld *= matY * matZ * m_matParent;
	memcpy(&matWorld.m[3], &vCenter, sizeof(_vec3));

	m_pColliderCom->Set_CollisionWorld(&matWorld);

	m_pColliderCom->Update_Collider();
}

void CWeapon::Collision_Att(const _tchar* pTargetTag)
{
	if (true == m_isAtt)
	{
		if (true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pColliderCom, pTargetTag, (CGameObject**)&m_pTargetUnit))
		{
			if (nullptr != m_pTargetUnit)
			{
				m_pTargetUnit->Add_Hp(m_pReleventUnit->Get_UnitInfo()->fAtt * -1.f);
				if (true == m_isKnockBack)
				{
					m_pTargetUnit->Knock_Back(m_fTimeDelta);
					m_isKnockBack = false;
				}
				if (!lstrcmp( L"Layer_Goblin", pTargetTag))
					((CGoblin*)m_pTargetUnit)->Set_Target(m_pReleventUnit);
			}

			m_isAtt = false;
			m_fAccAtt = 0.f;
		}
	}

	else
	{
		m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pColliderCom, pTargetTag);
		m_isAtt = false;
		m_fAccAtt = 0.f;
	}
}

_int CWeapon::Update_GameObject(const _float & fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;
	m_fAccAtt += fTimeDelta;

	_matrix		matPlayerWorld = *m_pRelevent_TransformCom->Get_WorldMatrix();
	m_matParent = (*m_pHandMatrix) * matPlayerWorld;

	if (m_fAccAtt > 0.2f)
	{
		Collision_Att(L"Layer_Goblin");
		Collision_Att(L"Layer_DemonLord");
		Collision_Att(L"Layer_BowlingBall_Full_Boss");
	}
	Update_Collider();

	return _RUN;
}

_int CWeapon::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UNIT, this);

	return _int();
}

void CWeapon::Render_GameObject(void)
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
		_MSG_BOX("CWeapon SetUp_ConstantTable Failed");
		return;
	}

	if (FAILED(pEffect->Begin(nullptr, 0)))
		int a = 0;

	m_pMeshCom->Render_Mesh(pEffect, 0);

	pEffect->End();	

	m_pColliderCom->Render_Collider();
}

void CWeapon::Free(void)
{
	if(nullptr != m_pReleventUnit)
		Safe_Release(m_pReleventUnit);
	if (nullptr != m_pTargetUnit)
		Safe_Release(m_pTargetUnit);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRelevent_TransformCom);
	Safe_Release(m_pObject_Manager);

	CGameObject::Free();
}

