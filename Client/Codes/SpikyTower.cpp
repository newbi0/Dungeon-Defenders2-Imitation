#include "stdafx.h"
#include "Defines.h"
#include "SpikyTower.h"

#include "Renderer.h"
#include "Mesh_Static.h"
#include "Mesh_Dynamic.h"
#include "Shader.h"
#include "Transform.h"
#include "Navigation.h"
#include "Collider.h"
#include "KeyChecker.h"
#include "HpBar_Enemy.h"
#include "HpBarBG_Enemy.h"

#include "Object_Manager.h"
#include "Sound_Device.h"
#include "Turn.h"
#include "Unit.h"
#include "Player.h"

#include "Camera_Follow.h"

CSpikyTower::CSpikyTower(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUnit(pGraphic_Device)
{
}

CSpikyTower::CSpikyTower(const CSpikyTower & rhs)
	: CUnit(rhs)
	, m_vMin(rhs.m_vMin)
	, m_vMax(rhs.m_vMax)
	, m_isAtt(false)
	, m_fTimeDelta(0.f)
{
}

_bool CSpikyTower::Check_CrushToBackOfTower(void)
{
	return m_pBackColliderCom->Get_isCollision();
}

void CSpikyTower::Add_Hp(const _float & fHpAmount)
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

HRESULT CSpikyTower::Ready_GameObject_Prototype(void)
{
	Init_UnitInfo(100.f, 10.f, 0.f);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Scaling(0.02f, 0.02f, 0.02f);

	//Min, Max
	m_vMin = *m_pMeshCom->Get_Min();
	m_vMax = *m_pMeshCom->Get_Max();

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	SetUp_LightAndMaterialInfo_OnShader(pEffect);

	return NOERROR;
}

HRESULT CSpikyTower::Ready_GameObject_Clone(const _vec3* pPos)
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

	m_pAttColliderCom = (Engine::CCollider*)Get_Component(L"Com_AttCollider");
	m_pAttColliderCom->AddRef();

	m_pBackColliderCom = (Engine::CCollider*)Get_Component(L"Com_BackCollider");
	m_pBackColliderCom->AddRef();

	m_pNavigationCom = (Engine::CNavigation*)Get_Component(L"Com_Navigation");
	m_pNavigationCom->AddRef();

	m_pKeyCheckerCom = (Engine::CKeyChecker*)Get_Component(L"Com_KeyChecker");
	m_pKeyCheckerCom->AddRef();

	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pObject_Manager->AddRef();

	m_pSound_Device = CSound_Device::Get_Instance();
	m_pSound_Device->AddRef();

	m_pColliderCom->Set_Collider(CCollider::TYPE_OBB, &m_vMin, &m_vMax, m_pTransformCom);
	m_pAttColliderCom->Set_Collider(CCollider::TYPE_OBB, &m_vMin, &_vec3(m_vMax.x, m_vMax.y, 2.f), m_pTransformCom);
	m_pBackColliderCom->Set_Collider(CCollider::TYPE_OBB, &_vec3(m_vMin.x, m_vMin.y, m_vMax.z), &_vec3(m_vMax.x, m_vMax.y, m_vMax.z + 100.f), m_pTransformCom);

	//HpBar 생성
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_HpBarBG_Enemy", L"Object_HpBarBG_Enemy")))
		return E_FAIL;
	_uint	iHpNum = m_pObject_Manager->Get_GameObjectNum(SCENE_STAGE, L"Layer_HpBarBG_Enemy");
	m_pHpBarBG = ((CHpBarBG_Enemy*)m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_HpBarBG_Enemy", iHpNum - 1));
	if (nullptr == m_pHpBarBG)
		return E_FAIL;
	m_pHpBarBG->AddRef();
	m_pHpBarBG->Set_UnitHpBar(this);

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_HpBar_Enemy", L"Object_HpBar_Enemy")))
		return E_FAIL;
	m_pHpBar = ((CHpBar_Enemy*)m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_HpBar_Enemy", iHpNum - 1));
	if (nullptr == m_pHpBar)
		return E_FAIL;
	m_pHpBar->AddRef();
	m_pHpBar->Set_UnitHpBar(this);

	return NOERROR;
}

HRESULT CSpikyTower::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	return NOERROR;
}

HRESULT CSpikyTower::Ready_Component(void)
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
	m_pMeshCom = (Engine::CMesh_Static*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Mesh_SpikyTower");
	if (nullptr == m_pMeshCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Mesh", m_pMeshCom);
	
	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Shader_Mesh");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	//For.Com_Collider
	m_pBackColliderCom = (Engine::CCollider*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Collider");
	if (nullptr == m_pBackColliderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_BackCollider", m_pBackColliderCom);

	m_pColliderCom = (Engine::CCollider*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Collider");
	if (nullptr == m_pColliderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Collider", m_pColliderCom);

	m_pAttColliderCom = (Engine::CCollider*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Collider");
	if (nullptr == m_pAttColliderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_AttCollider", m_pAttColliderCom);

	// For.Com_Navigation
	m_pNavigationCom = (Engine::CNavigation*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Navigation");
	if (nullptr == m_pNavigationCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Navigation", m_pNavigationCom);

	// For.Com_KeyChecker
	m_pKeyCheckerCom = (Engine::CKeyChecker*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_KeyChecker");
	if (nullptr == m_pKeyCheckerCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_KeyChecker", m_pKeyCheckerCom);

	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pObject_Manager->AddRef();

	m_pSound_Device = CSound_Device::Get_Instance();
	m_pSound_Device->AddRef();

	return NOERROR;
}

HRESULT CSpikyTower::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

void CSpikyTower::Update_Collider(void)
{
	m_pColliderCom->Update_Collider();
	m_pAttColliderCom->Update_Collider();
	m_pBackColliderCom->Update_Collider();
}

void CSpikyTower::Collision_Att(void)
{
	// 5초에 한번씩 공격
	if (m_fTimeDelta > 5.f)
	{
		_uint iGoblinNum = m_pObject_Manager->Get_GameObjectNum(SCENE_STAGE, L"Layer_Goblin");
		for (_uint i = 0; i < iGoblinNum; ++i)
		{
			CCollider*	pClubCollider = (CCollider*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Goblin", i)->Get_Component(L"Com_ClubCollider"));
			if (true == m_pAttColliderCom->Collision_OBB(&pClubCollider))
			{
				m_pTargetUnit = (CUnit*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Goblin", i));
				m_isAtt = true;
				break;
			}
		}

		if (true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pAttColliderCom, L"Layer_Goblin", (CGameObject**)&m_pTargetUnit))
		{
			m_isAtt = true;
		}
		m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pColliderCom, L"Layer_Player");
	}
	else
	{
		m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pColliderCom, L"Layer_Goblin");
		m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pColliderCom, L"Layer_Player");
	}

	if (true == m_isAtt)
	{
		m_fTimeDelta = 0.f;
		if (nullptr != m_pTargetUnit)
		{
			m_pTargetUnit->Set_Knock_Back();
			m_pTargetUnit->Add_Hp(-50.f);
		}

		m_isAtt = false;
	}

}

_int CSpikyTower::Update_GameObject(const _float & fTimeDelta)
{
	if (true == m_isDeadAni)
	{
		m_pHpBarBG->Set_isDead(true);
		m_pHpBar->Set_isDead(true);
		m_pAttColliderCom->Set_isTargetDead(true);
		m_pBackColliderCom->Set_isTargetDead(true);
		m_pColliderCom->Set_isTargetDead(true);
		Set_isDead(true);
	}
	else
	{
		m_fTimeDelta += fTimeDelta;

		Update_Collider();

		Collision_Att();

		CCollider*	pTargetCollider = (CCollider*)m_pObject_Manager->Get_Component(SCENE_STAGE, L"Layer_Player", L"Com_Collider");
		if (nullptr != pTargetCollider)
			m_pBackColliderCom->Collision_OBB(&pTargetCollider, false);

		//Bulid
		if (false == m_isBuild)
		{
			((CCamera_Follow*)m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Camera"))->Set_MouseMove(false);

			if (false == m_isPos)
			{
				_vec3 vMousePos = m_pNavigationCom->Compute_MousePosOnCell(g_hWnd);
				m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &vMousePos);

				if (true == m_pKeyCheckerCom->Check_MouseButton_Once(CKeyChecker::DIM_LBUTTON))
				{
					m_pSound_Device->SoundPlay(27, 0);
					m_isPos = true;
				}
			}
			else
			{
				if (_long lMouseMoveX = m_pInput_Device->Get_MouseMove(CInput_Device::DIMM_X))
					m_pTransformCom->RotationY(lMouseMoveX * fTimeDelta);

				//Bulid off
				if (true == m_pKeyCheckerCom->Check_MouseButton_Once(CKeyChecker::DIM_LBUTTON))
				{
					((CCamera_Follow*)m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Camera"))->Set_MouseMove(true);
					((CPlayer*)m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"))->Set_isBuild(false);
					m_isBuild = true;
					m_pSound_Device->SoundPlay(27, 0);
				}
			}
		}
	}

	return _RUN;
}

_int CSpikyTower::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return _int();
}

void CSpikyTower::Render_GameObject(void)
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
		_MSG_BOX("CSpikyTower SetUp_ConstantTable Failed");
		return;
	}

	if (FAILED(pEffect->Begin(nullptr, 0)))
		int a = 0;

	m_pMeshCom->Render_Mesh(pEffect, 0);

	pEffect->End();	

	m_pColliderCom->Render_Collider();
	//m_pAttColliderCom->Render_Collider();
	//m_pBackColliderCom->Render_Collider();
}

CSpikyTower * CSpikyTower::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSpikyTower* pInstance = new CSpikyTower(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CSpikyTower Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpikyTower::Clone(const _vec3 * pPos)
{

	CGameObject*	pInstance = new CSpikyTower(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CSpikyTower Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSpikyTower::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CSpikyTower(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CSpikyTower Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSpikyTower::Free(void)
{
	if (nullptr != m_pTargetUnit)
		Safe_Release(m_pTargetUnit);

	Safe_Release(m_pHpBarBG);
	Safe_Release(m_pHpBar);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pAttColliderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pKeyCheckerCom);

	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pSound_Device);

	CUnit::Free();
}

