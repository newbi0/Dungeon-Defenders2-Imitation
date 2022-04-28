#include "stdafx.h"
#include "Defines.h"
#include "DemonLord.h"

#include "Renderer.h"
#include "Mesh_Dynamic.h"
#include "Shader.h"
#include "Transform.h"
#include "Navigation.h"
#include "Collider.h"
#include "AStar.h"

#include "Camera_Follow.h"
#include "Input_Device.h"
#include "Object_Manager.h"
#include "Sound_Device.h"
#include "Turn.h"
#include "HpBar_Boss.h"
#include "HpBarBG_Boss.h"
#include "HpBarFR_Boss.h"
#include "SkillBar_Boss.h"
#include "SkillBarBG_Boss.h"
#include "Font_Damage.h"

#include "FireBall.h"
#include "Wind.h"
#include "SpikyTower.h"
#include "Player.h"
#include "Cursor.h"

const float DAMAGE_HIGHT = 2.f;
const float SPEED = 5.f;
const float JUMP_HIGHT = 5.f;
const float JUMP_END_POINT = 0.2f;
const float JUMP_UP_SPEED = 4.f;
const float JUMP_DOWN_SPEED = 2.f;
const float JUMP_DOWN_SPEED_SKILL3 = 10.f;
const float HITZONE_SIZE = 40.f;

CDemonLord::CDemonLord(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUnit(pGraphic_Device)
{
}

CDemonLord::CDemonLord(const CDemonLord & rhs)
	: CUnit(rhs)
	, m_matRotY(rhs.m_matRotY)
	, m_iCurrentIdx(rhs.m_iCurrentIdx)
	, m_vecAniInfo(rhs.m_vecAniInfo)
	, m_vCrystalPos(rhs.m_vCrystalPos)
	, m_isAtt(false)
	, m_isAttOnce(false)
	, m_vTargetPos(rhs.m_vTargetPos)
	, m_isEnter(false)
	, m_isJumpUp(true)
	, m_isAniPlay(true)
{
}

const _matrix * CDemonLord::Get_WorldMatrix(void) const
{
	if (nullptr == m_pTransformCom)
	{
		_matrix	matIden;
		return D3DXMatrixIdentity(&matIden);
	}
	return m_pTransformCom->Get_WorldMatrix();
}

const _matrix * CDemonLord::Get_matRotY(void) const
{
	return &m_matRotY;
}

void CDemonLord::Set_Target(CUnit * pTarget)
{
	if (nullptr == pTarget)
		return;
	if (nullptr != m_pTargetUnit)
		Safe_Release(m_pTargetUnit);

	m_isFollower = true;

	m_pTargetUnit = pTarget;
	m_pTargetUnit->AddRef();

	CTransform*	pTargetTransform = dynamic_cast<CTransform*>(pTarget->Get_Component(L"Com_Transform"));
	if (nullptr == pTargetTransform)
		return;
	m_vTargetPos = *pTargetTransform->Get_Infomation(CTransform::INFO_POSITION);

	if (nullptr == m_pTransformCom)
		return;
	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	m_listBest = m_pAStarCom->Make_Rute(m_iCurrentIdx, vPos, m_vTargetPos);
}

void CDemonLord::Add_Hp(const _float & fHpAmount)
{
	if (false == m_isDeadAni
		&& true == m_isAttacked)
	{
		if (fHpAmount < 0)
		{
			Set_Animation(Ani_Hurt);

			_float	fDamege = fHpAmount + m_tUnitInfo.fDef;
			m_tUnitInfo.fHp += fDamege;

			//Blood
			if (nullptr != m_pTransformCom
				&& nullptr != m_pObject_Manager)
				m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Blood", L"Object_Blood", m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION));

			//Font
			_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
			if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Font_Damage", L"Object_Font_Damage", &_vec3(vPos.x, vPos.y + DAMAGE_HIGHT, vPos.z))))
				return;
			CFont_Damage* pFont = dynamic_cast<CFont_Damage*>(m_pObject_Manager->Get_LastGameObject(SCENE_STAGE, L"Layer_Font_Damage"));
			if (nullptr != pFont)
				pFont->Set_Font((_uint)(fDamege * -1.f));
		}
		else
			m_tUnitInfo.fHp += fHpAmount;

		if (m_tUnitInfo.fMaxHp < m_tUnitInfo.fHp)
			m_tUnitInfo.fHp = m_tUnitInfo.fMaxHp;

		if (0.f >= m_tUnitInfo.fHp)
		{
			if (nullptr != m_pColliderCom
				&& nullptr != m_pSound_Device)
			{
				m_pColliderCom->Set_isTargetDead(true);
				m_pSound_Device->SoundPlay(9, 0);
			}
			m_isDeadAni = true;
		}
	}
}

HRESULT CDemonLord::Ready_GameObject_Prototype(void)
{
	Init_UnitInfo(500.f, 50.f, 0.f);

	m_vCrystalPos = _vec3(85.0352554f, 0.f, 115.778961f);

	if (FAILED(Ready_Component())
		|| FAILED(Ready_AniInfo()))
		return E_FAIL;

	HANDLE	hFile;

	hFile = CreateFile(L"../Bin/Data/DemonZenPos.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	_ulong dwByte = 0;

	for (;;)
	{
		_vec3	pPos;

		ReadFile(hFile, &pPos, sizeof(_vec3), &dwByte, 0);
		if (dwByte == 0)
			break;
		m_vTargetPos = pPos;
	}

	CloseHandle(hFile);

	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(94.7387f, 3.0462f, 77.0904f));
	m_pTransformCom->Scaling(0.25f, 0.25f, 0.25f);
	m_pTransformCom->RotationY(90.f);
	D3DXMatrixIdentity(&m_matRotY);

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	SetUp_LightAndMaterialInfo_OnShader(pEffect);

	return NOERROR;
}

HRESULT CDemonLord::Ready_GameObject_Clone(const _vec3* pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	if (nullptr == m_pRendererCom)
		return E_FAIL;
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	if (nullptr == m_pTransformCom)
		return E_FAIL;
	m_pTransformCom->AddRef();

	m_pMeshCom = (Engine::CMesh_Dynamic*)Get_Component(L"Com_Mesh");
	if (nullptr == m_pMeshCom)
		return E_FAIL;
	m_pMeshCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	m_pShaderCom->AddRef();

	m_pNaviCom = (Engine::CNavigation*)Get_Component(L"Com_Navigation");
	if (nullptr == m_pNaviCom)
		return E_FAIL;
	m_pNaviCom->AddRef();

	m_pColliderCom = (Engine::CCollider*)Get_Component(L"Com_Collider");
	if (nullptr == m_pColliderCom)
		return E_FAIL;
	m_pColliderCom->AddRef();

	m_pLeftColliderCom = (Engine::CCollider*)Get_Component(L"Com_LeftCollider");
	if (nullptr == m_pLeftColliderCom)
		return E_FAIL;
	m_pLeftColliderCom->AddRef();

	m_pRightColliderCom = (Engine::CCollider*)Get_Component(L"Com_RightCollider");
	if (nullptr == m_pRightColliderCom)
		return E_FAIL;
	m_pRightColliderCom->AddRef();

	m_pAStarCom = (Engine::CAStar*)Get_Component(L"Com_AStar");
	if (nullptr == m_pAStarCom)
		return E_FAIL;
	m_pAStarCom->AddRef();

	m_pObject_Manager = CObject_Manager::Get_Instance();
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	m_pObject_Manager->AddRef();

	m_pSound_Device = CSound_Device::Get_Instance();
	if (nullptr == m_pSound_Device)
		return E_FAIL;
	m_pSound_Device->AddRef();

	if(nullptr != pPos)
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, pPos);

	Set_Animation(Ani_Fly_Forward);
 	m_pNaviCom->Compute_CurrentIndex(*m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), &m_iCurrentIdx);

	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	_vec3	vMin, vMax;

	//로컬 좌표
	m_vMin = _vec3(-8.f, 0.f, -8.f);
	m_vMax = _vec3(8.f, 35.f, 8.f);

	m_pColliderCom->Set_Collider(CCollider::TYPE_AABB, &m_vMin, &m_vMax, m_pTransformCom);

	vMin = _vec3(0.f, -1.f, 0.f);
	vMax = _vec3(4.f, 18.f, 1.f);
	m_pLeftColliderCom->Set_Collider(CCollider::TYPE_OBB, &vMin, &vMax, m_pTransformCom);
	m_pRightColliderCom->Set_Collider(CCollider::TYPE_OBB, &vMin, &vMax, m_pTransformCom);

	//HpBar
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_HpBarBG_Boss", L"Object_HpBarBG_Boss")))
		return E_FAIL;
	_uint	iHpNum = m_pObject_Manager->Get_GameObjectNum(SCENE_STAGE, L"Layer_HpBarBG_Boss");
	m_pHpBarBG = ((CHpBarBG_Boss*)m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_HpBarBG_Boss", iHpNum - 1));
	if (nullptr == m_pHpBarBG)
		return E_FAIL;
	m_pHpBarBG->AddRef();

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_HpBar_Boss", L"Object_HpBar_Boss")))
		return E_FAIL;
	m_pHpBar = ((CHpBar_Boss*)m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_HpBar_Boss", iHpNum - 1));
	if (nullptr == m_pHpBar)
		return E_FAIL;
	m_pHpBar->AddRef();
	m_pHpBar->Set_UnitHpBar(this);

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_HpBarFR_Boss", L"Object_HpBarFR_Boss")))
		return E_FAIL;
	m_pHpBarFR = ((CHpBarFR_Boss*)m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_HpBarFR_Boss", iHpNum - 1));
	if (nullptr == m_pHpBarFR)
		return E_FAIL;
	m_pHpBarFR->AddRef();

	return NOERROR;
}

HRESULT CDemonLord::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	return NOERROR;
}

HRESULT CDemonLord::Ready_Component(void)
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

	// For.Com_Navigation
	m_pNaviCom = (Engine::CNavigation*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Navigation");
	if (nullptr == m_pNaviCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Navigation", m_pNaviCom);

	// For.Com_Mesh
	m_pMeshCom = (Engine::CMesh_Dynamic*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Mesh_DemonLord");
	if (nullptr == m_pMeshCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Mesh", m_pMeshCom);
	
	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Shader_Unit");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	// For.Com_Collider
	m_pColliderCom = (Engine::CCollider*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Collider");
	if (nullptr == m_pColliderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Collider", m_pColliderCom);

	m_pLeftColliderCom = (Engine::CCollider*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Collider");
	if (nullptr == m_pLeftColliderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_LeftCollider", m_pLeftColliderCom);

	m_pRightColliderCom = (Engine::CCollider*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Collider");
	if (nullptr == m_pRightColliderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_RightCollider", m_pRightColliderCom);

	// For.Com_Collider
	m_pAStarCom = (Engine::CAStar*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_AStar");
	if (nullptr == m_pAStarCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_AStar", m_pAStarCom);

	m_pObject_Manager = CObject_Manager::Get_Instance();
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	m_pObject_Manager->AddRef();

	m_pSound_Device = CSound_Device::Get_Instance();
	if (nullptr == m_pSound_Device)
		return E_FAIL;
	m_pSound_Device->AddRef();

	return NOERROR;
}

HRESULT CDemonLord::Ready_AniInfo(void)
{
	m_vecAniInfo.reserve(Ani_END);

	ANIINFO*	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_TurnLeft;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Shock_Ground;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Shock_Air;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_MoveForward;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Idle;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Hurt;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 2.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Fly_Right;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Fly_Pound_Loop;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Fly_Pound_End;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Fly_Left;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Fly_Launch;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);


	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Fly_Hover;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);


	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Fly_Forward;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);


	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Fly_Fireball;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 2.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Fly_Backward;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Death;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Cinematic_Landing;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 2.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Cinematic_Cutthroat;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 2.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);


	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Attack_SummonFlame;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);


	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Attack_Slice;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Attack_Right;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 2.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Attack_Pound;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 3.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Attack_Left;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 2.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);


	return NOERROR;
}

HRESULT CDemonLord::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

void CDemonLord::Set_Animation(const eANINAME & eAniName)
{
	if (nullptr == m_pMeshCom
		|| Ani_END == eAniName)
		return;

	m_pMeshCom->SetUp_AnimationSet(eAniName, m_vecAniInfo[eAniName]->fWeight);
	m_pMeshCom->Set_AdjustPeriod(m_vecAniInfo[eAniName]->dPeriode);
}

void CDemonLord::Set_Direction(const _vec3 & vDir, const _bool& isFront, const _float& fDistance)
{
	if (nullptr == m_pTransformCom)
		return;

	_vec3	vNormalLook;
	D3DXVec3Normalize(&vNormalLook, m_pTransformCom->Get_Infomation(CTransform::INFO_LOOK));
	_vec3	vDir_New;
	D3DXVec3Normalize(&vDir_New, &vDir);
	_float	fCos = D3DXVec3Dot(&vNormalLook, &vDir_New);
	fCos = acosf(-fCos);

	if (false == isnan(fCos) && fDistance < fCos)
	{
		if (false == isFront)
			m_pTransformCom->RotationY(fCos);
		else
			m_pTransformCom->RotationY(-fCos);
	}

}

void CDemonLord::Update_Collider(void)
{
	if (nullptr == m_pObject_Manager
		|| nullptr == m_pColliderCom
		|| nullptr == m_pLeftColliderCom
		|| nullptr == m_pRightColliderCom)
		return;

	//if (true == m_pObject_Manager->Check_Collision_AABB(SCENE_STAGE, m_pColliderCom, L"Layer_Player") ||
	//	true == m_pObject_Manager->Check_Collision_AABB(SCENE_STAGE, m_pColliderCom, L"Layer_Mesh_Core_Crystal_0"))
	//	m_isAtt = true;

	m_pObject_Manager->Check_Collision_AABB(SCENE_STAGE, m_pColliderCom, L"Layer_Goblin");

	//오른검 콜라이더 World Setting
	_matrix matParent_R = *m_pMeshCom->Get_FrameCombinedMatrix("b_HandR") * *m_pTransformCom->Get_WorldMatrix();

	_vec3	vCenter = *(_vec3*)matParent_R.m[3];
	vCenter.y -= 1.f;

	_matrix	matWorld, matY, matZ;
	D3DXMatrixIdentity(&matWorld);

	D3DXMatrixRotationX(&matY, 0.f);
	D3DXMatrixRotationZ(&matZ, -3.f);

	matWorld *= matY * matZ * matParent_R;
	memcpy(&matWorld.m[3], &vCenter, sizeof(_vec3));

	m_pRightColliderCom->Set_CollisionWorld(&matWorld);

	//왼검 콜라이더 World Setting
	_matrix matParent_L = *m_pMeshCom->Get_FrameCombinedMatrix("b_HandL") * *m_pTransformCom->Get_WorldMatrix();

	vCenter = *(_vec3*)matParent_L.m[3];

	D3DXMatrixIdentity(&matWorld);

	D3DXMatrixRotationY(&matY, 0.f);
	D3DXMatrixRotationZ(&matZ, -0.1f);

	matWorld *= matY * matZ * matParent_L;
	memcpy(&matWorld.m[3], &vCenter, sizeof(_vec3));

	m_pLeftColliderCom->Set_CollisionWorld(&matWorld);

	m_pColliderCom->Update_Collider();
	m_pLeftColliderCom->Update_Collider();
	m_pRightColliderCom->Update_Collider();
}

void CDemonLord::Collider_Att(void)
{
	if (nullptr == m_pObject_Manager
		|| nullptr == m_pLeftColliderCom
		|| nullptr == m_pRightColliderCom
		|| nullptr == m_pTargetUnit)
		return;

	if (true == m_isAttOnce)
	{
		CUnit*	pTmp = nullptr;
		if(nullptr != m_pTargetUnit)
			pTmp = m_pTargetUnit;

			//왼손
		if (true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pLeftColliderCom, L"Layer_Player", (CGameObject**)&m_pTargetUnit)
			//오른손
			|| true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pRightColliderCom, L"Layer_Player", (CGameObject**)&m_pTargetUnit))
		{
			Safe_Release(pTmp);

			_vec3	vDir = *((CTransform*)m_pTargetUnit->Get_Component(L"Com_Transform"))->Get_Infomation(CTransform::INFO_POSITION) - *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
			Set_Direction(vDir);

			if (nullptr != m_pTargetUnit)
			{
				m_pTargetUnit->Add_Hp(m_tUnitInfo.fAtt * -1.f);
				m_pTargetUnit->AddRef();
			}

			m_isAttOnce = false;
		}
	}

	else
	{
		m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pLeftColliderCom, L"Layer_Player");
		m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pRightColliderCom, L"Layer_Player");
	}
}

void CDemonLord::Att_Basic(const _float & fTimeDelta)
{
	if (nullptr == m_pObject_Manager
		|| nullptr == m_pSound_Device
		|| nullptr == m_pMeshCom)
		return;

	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

	if (0 != m_pObject_Manager->Get_GameObjectNum(SCENE_STAGE, L"Layer_Player"))
	{
		const CTransform*	pPlayerTransform = dynamic_cast<const CTransform*>(m_pObject_Manager->Get_Component(SCENE_STAGE, L"Layer_Player", L"Com_Transform"));
		if (nullptr == pPlayerTransform)
			return;
		_vec3	vPlayerPos = *pPlayerTransform->Get_Infomation(CTransform::INFO_POSITION);
		_vec3	vDir = vPlayerPos - vPos;

		//플레이어가 가까이 있을 때 근거리 공격
		if (D3DXVec3Length(&vDir) < 10.f
			&& false == m_isInitSkill0)
		{
			if (Ani_Idle == m_pMeshCom->Get_CurrentAniIdx())
			{
				m_pSound_Device->SoundPlay(6, 0);
				Set_Animation(Ani_Attack_Left);
			}
			else if (Ani_Attack_Right == m_pMeshCom->Get_CurrentAniIdx()
				&& true == m_pMeshCom->is_Finished())
			{
				m_pSound_Device->SoundPlay(6, 0);
				Set_Animation(Ani_Attack_Left);
			}
			else if (Ani_Attack_Left == m_pMeshCom->Get_CurrentAniIdx()
				&& true == m_pMeshCom->is_Finished())
			{
				m_pSound_Device->SoundPlay(6, 0);
				Set_Animation(Ani_Attack_Right);
			}
		}

		//멀리 있을 때 원거리 공격
		else
		{
			Skill_0(fTimeDelta);
		}
	}

}

void CDemonLord::Skill_0(const _float & fTimeDelta)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pSound_Device
		|| nullptr == m_pMeshCom
		|| nullptr == m_pObject_Manager)
		return;

	if (false == m_isInitSkill0)
	{
		m_pSound_Device->SoundPlay(7, 0);
		m_isAttacked = false;
		Set_Animation(Ani_Fly_Launch);
		m_isJump = true;
		m_isJumpUp = true;
		m_fOriginalY = (*m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION)).y;
		m_isInitSkill0 = true;
	}

	if (true == m_isJump)
	{
		_float fJumpY = (*m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION)).y;
		if (true == m_isJumpUp)
		{
			//높이차이 5.f 이상이면 불 발사
			if (fJumpY - m_fOriginalY > JUMP_HIGHT)
			{
				if (Ani_Fly_Launch == m_pMeshCom->Get_CurrentAniIdx()
					&& true == m_pMeshCom->is_Finished())
				{
					const CTransform*		pTargetTransform = dynamic_cast<const CTransform*>((m_pObject_Manager->Get_Component(SCENE_STAGE, L"Layer_Player", L"Com_Transform")));
					if (nullptr == pTargetTransform)
						return;
					_vec3	vTargetPos = *pTargetTransform->Get_Infomation(CTransform::INFO_POSITION);
					if (vTargetPos == _vec3(0.f, 0.f, 0.f))
						return;
					if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_FireBall", L"Object_FireBall", m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION))))
						return;
					CFireBall*	pEffect = dynamic_cast<CFireBall*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_FireBall"));
					if (nullptr != pEffect)
						pEffect->Set_TargetPos(vTargetPos);

					Set_Animation(Ani_Fly_Fireball);
				}

				else if (Ani_Fly_Fireball == m_pMeshCom->Get_CurrentAniIdx()
					&& true == m_pMeshCom->is_Finished())
				{
					Set_Animation(Ani_Fly_Hover);
					m_isJumpUp = false;
				}
			}
			//이하면 점프
			else
			{
				const CTransform*		pTargetTransform = dynamic_cast<const CTransform*>(m_pObject_Manager->Get_Component(SCENE_STAGE, L"Layer_Player", L"Com_Transform"));
				if (nullptr == pTargetTransform)
					return;
				_vec3	vDir = *pTargetTransform->Get_Infomation(CTransform::INFO_POSITION)
					- *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
				Set_Direction(vDir, true);
				m_pTransformCom->Jump(fTimeDelta * JUMP_UP_SPEED);
			}
		}
		else
		{
			//내려오기
			m_pTransformCom->Jump(fTimeDelta * JUMP_DOWN_SPEED * -1.f);
			if (fJumpY - m_fOriginalY < JUMP_END_POINT)
			{
				Set_Animation(Ani_Idle);
				m_isJump = false;
				m_isInitSkill0 = false;
				m_isAttacked = true;
			}
		}
	}
}

void CDemonLord::Skill_1(const _float & fTimeDelta)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pMeshCom
		|| nullptr == m_pSound_Device
		|| nullptr == m_pObject_Manager)
		return;

	if (false == m_isSkill1)
	{
		_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

		if (false == m_isInitSkill1)
		{
			m_pSound_Device->SoundPlay(11, 0);
			Set_Animation(Ani_Attack_SummonFlame);
			m_pObject_Manager->Set_HitZone(CGameObject::TYPE_CIRCLE, SCENE_STAGE);
			m_isInitSkill1 = true;
			m_isAttacked = false;
		}

		if (Ani_Attack_SummonFlame == m_pMeshCom->Get_CurrentAniIdx()
			&& true == m_pMeshCom->is_Finished())
			m_isAniPlay = false;

		if (true == m_pObject_Manager->Get_isHitZone())
		{
			const CTransform*	pPlayerTransform = dynamic_cast<const CTransform*>(m_pObject_Manager->Get_Component(SCENE_STAGE, L"Layer_Player", L"Com_Transform"));
			if (nullptr == pPlayerTransform)
				return;
			_vec3	vPlayerPos = *(pPlayerTransform->Get_Infomation(CTransform::INFO_POSITION));
			_vec3	vPosXZ = vPos;
			vPosXZ.y = 0.f;
			vPlayerPos.y = 0.f;
			_float	fDistance = D3DXVec3Length(&(vPosXZ - vPlayerPos));
			if (HITZONE_SIZE * 0.5f > fDistance)
			{
				m_pSound_Device->SoundPlay(12, 0);
				CUnit*	pPlayer = dynamic_cast<CUnit*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"));
				pPlayer->Add_Hp(m_tUnitInfo.fAtt * -1.f);
				m_isAttacked = true;
			}
			m_isSkill1 = true;
			m_isInitSkill0 = false;
			Set_Animation(Ani_Attack_Pound);
			m_isAniPlay = true;

			CCamera_Follow*	pCamera = dynamic_cast<CCamera_Follow*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Camera"));
			pCamera->Set_isShake(true, 0.3f, 3);
		}
	}

}

void CDemonLord::Skill_2(const _float & fTimeDelta)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pSound_Device
		|| nullptr == m_pObject_Manager)
		return;

	if (false == m_isSkill2)
	{
		if (false == m_isInitSkill2)
		{
			m_pSound_Device->SoundPlay(13, 0);
			m_isInitSkill2 = true;
			m_isAttacked = false;

			Set_Animation(Ani_Attack_SummonFlame);

			_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

			_vec3	vObjectPos = _vec3(vPos.x - 5.f, vPos.y + 3.f, vPos.z + 5.f);

			if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_BossBackAura", L"Object_BossBackAura", &_vec3(vPos.x, vPos.y + 11.f, vPos.z))))
				return;

			if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_BossSkillAura", L"Object_BossSkillAura", &vObjectPos)))
				return;

			if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_BowlingBall_Full_Boss", L"Object_BowlingBall_Full_Boss", &vObjectPos)))
				return;

			CTransform*	pObjectTransform = (CTransform*)(m_pObject_Manager->Get_Component(SCENE_STAGE, L"Layer_BowlingBall_Full_Boss", L"Com_Transform"));
			if (nullptr != pObjectTransform)
				pObjectTransform->Scaling(0.05f, 0.05f, 0.05f);

			//SkillBar
			if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_SkillBar_Boss", L"Object_SkillBar_Boss")))
				return;
			CSkillBar_Boss*	pSkillBar = dynamic_cast<CSkillBar_Boss*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_SkillBar_Boss"));
			if (nullptr == pSkillBar)
				return;
			pSkillBar->Set_UnitHpBar(this);

			if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_SkillBarBG_Boss", L"Object_SkillBarBG_Boss")))
				return;
		}

		m_fTimeAcc += fTimeDelta;
		m_fGage = m_fTimeAcc / 10.f;

		//성공
		if (false == m_isShock)
		{
			if (m_fTimeAcc < 10.f)
			{
				if (0 == m_pObject_Manager->Get_GameObjectNum(SCENE_STAGE, L"Layer_BowlingBall_Full_Boss"))
				{
					Set_Animation(Ani_Shock_Ground);
					m_fTimeAcc = 0;
					m_isShock = true;

					m_pObject_Manager->Clear_GameObject(SCENE_STAGE, L"Layer_SkillBar_Boss");
					m_pObject_Manager->Clear_GameObject(SCENE_STAGE, L"Layer_SkillBarBG_Boss");
					m_pObject_Manager->Clear_GameObject(SCENE_STAGE, L"Layer_BossBackAura");

					m_pSound_Device->SoundPlay(10, 0);
				}
			}
			//실패
			else
			{
				CUnit* pPlayer = dynamic_cast<CUnit*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"));
				CCamera_Follow*	pCamera = dynamic_cast<CCamera_Follow*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Camera"));
				CGameObject*	pBall = m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_BowlingBall_Full_Boss");
				CGameObject*	pBallEffect = m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_BossSkillAura");
				CGameObject*	pBossEffect = m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_BossBackAura");

				if (nullptr != pPlayer)
					pPlayer->Add_Hp(m_tUnitInfo.fAtt * -3.f);
				if (nullptr != pCamera)
					pCamera->Set_isShake(true, 0.3f, 3);
				if (nullptr != pBall)
					pBall->Set_isDead(true);
				if (nullptr != pBallEffect)
					pBallEffect->Set_isDead(true);
				if (nullptr != pBossEffect)
					pBossEffect->Set_isDead(true);

				for (_uint i = 0; i < 10; ++i)
				{
					for (_uint j = 0; j < 10; ++j)
					{
						if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Thunderbolt", L"Object_Thunderbolt", &_vec3(80.f + (10.f * i), -5.f, 80.f + (10.f * j)))))
							return;
					}
				}

				Set_Animation(Ani_Attack_Pound);
				m_isSkill2 = true;
				m_isInitSkill2 = false;
				m_fTimeAcc = 0;
				m_isAttacked = true;

				m_pObject_Manager->Clear_GameObject(SCENE_STAGE, L"Layer_SkillBar_Boss");
				m_pObject_Manager->Clear_GameObject(SCENE_STAGE, L"Layer_SkillBarBG_Boss");
			}
		}

		if (true == m_isShock)
		{
			Set_Animation(Ani_Shock_Ground);
			if (m_fTimeAcc > 3.f)
			{
				m_isShock = false;
				m_isSkill2 = true;
				m_isInitSkill2 = false;
				m_fTimeAcc = 0;
				m_isAttacked = true;
				m_fTimeAcc = true;
			}
		}
	}
}

void CDemonLord::Skill_3(const _float & fTimeDelta)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pSound_Device)
		return;

	if (false == m_isSkill3)
	{
		if (false == m_isInitSkill3)
		{
			m_pSound_Device->SoundPlay(16, DSBPLAY_LOOPING);
			Set_Animation(Ani_Fly_Pound_Loop);
			m_isJump = true;
			m_isJumpUp = true;
			m_isInitSkill3 = true;
			m_isAttacked = false;

			_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
			m_fOriginalY = vPos.y;

			//Effect
			if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Wind", L"Object_Wind", &_vec3(vPos.x, vPos.y + 5.f, vPos.z))))
				return;
		}

		if (true == m_isJump)
		{
			_float fJumpY = (*m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION)).y;
			if (true == m_isJumpUp)
			{
				//다 올라 갔을 때
				if (fJumpY - m_fOriginalY > JUMP_HIGHT)
				{
					Set_Animation(Ani_Fly_Pound_Loop);
					m_isJumpUp = false;
					m_isGage = true;

					//SkillBar
					if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_SkillBar_Boss", L"Object_SkillBar_Boss")))
						return;
					CSkillBar_Boss*	pSkillBar = dynamic_cast<CSkillBar_Boss*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_SkillBar_Boss"));
					if (nullptr == pSkillBar)
						return;
					pSkillBar->Set_UnitHpBar(this);
					if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_SkillBarBG_Boss", L"Object_SkillBarBG_Boss")))
						return;
				}
				//이하면 점프
				else
					m_pTransformCom->Jump(fTimeDelta * 4.f);
			}

			else
			{
				if (true == m_isGage)
				{
					Set_Animation(Ani_Fly_Pound_Loop);
					m_fAccTimeDelta_ForSkill3 += fTimeDelta;
					m_fAccTimeDelta2_ForSkill3 += fTimeDelta;
					m_fGage = m_fAccTimeDelta_ForSkill3 / 10.f;

					//플레이어 끌어당기기
					if (m_fAccTimeDelta2_ForSkill3 > 2.f)
					{
						CPlayer*	pPlayer = dynamic_cast<CPlayer*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"));
						if (nullptr != pPlayer)
							pPlayer->Set_PullOut(false, _vec3(0.f, 0.f, 0.f), 10.f);
						if (m_fAccTimeDelta2_ForSkill3 > 5.f)
							m_fAccTimeDelta2_ForSkill3 = 0.f;
					}
					else
					{
						CSpikyTower*	pTower = dynamic_cast<CSpikyTower*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_SpikyTower"));
						if (pTower == nullptr
							|| false == pTower->Check_CrushToBackOfTower())
						{
							_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
							CPlayer*	pPlayer = dynamic_cast<CPlayer*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"));
							if (nullptr != pPlayer)
								pPlayer->Set_PullOut(true, vPos, 10.f);
						}
					}

					//게이지 다 차면
					if (m_fAccTimeDelta_ForSkill3 > 10.f)
					{
						CPlayer*	pPlayer = dynamic_cast<CPlayer*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"));
						if (nullptr != pPlayer)
							pPlayer->Set_PullOut(false, _vec3(0.f, 0.f, 0.f), 10.f);

						m_isGage = false;

						m_pObject_Manager->Clear_GameObject(SCENE_STAGE, L"Layer_SkillBar_Boss");
						m_pObject_Manager->Clear_GameObject(SCENE_STAGE, L"Layer_SkillBarBG_Boss");
					}
				}
				else
				{
					//내려오기
					Set_Animation(Ani_Fly_Pound_End);
					m_pTransformCom->Jump(-fTimeDelta * JUMP_DOWN_SPEED_SKILL3);
					if (fJumpY - m_fOriginalY < JUMP_END_POINT)
					{
						//플레이어가 안전지대가 아니면 바로 사망시키는 코드
						CSpikyTower*	pTower = dynamic_cast<CSpikyTower*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_SpikyTower"));						
						if (pTower == nullptr
							|| false == pTower->Check_CrushToBackOfTower())
						{
							CUnit*	pTarget = dynamic_cast<CUnit*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"));
							if (nullptr != pTarget)
								pTarget->Set_isDead(true);

							m_isJump = false;
							m_isAttacked = true;
							m_isSkill3 = true;
						}
						else
						{
							m_isJump = false;
							m_isAttacked = true;
							m_isSkill3 = true;
						}

						//효과
						for (_uint i = 0; i < 10; ++i)
						{
							for (_uint j = 0; j < 10; ++j)
							{
								if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Thunderbolt", L"Object_Thunderbolt", &_vec3(80.f + (10.f * i), -5.f, 80.f + (10.f * j)))))
									return;
							}
						}

						CWind*	pEffect = dynamic_cast<CWind*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Wind"));
						if(nullptr != pEffect)
							pEffect->Set_Disappear(true);

						CCamera_Follow*	pCamera = dynamic_cast<CCamera_Follow*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Camera"));
						if (nullptr != pCamera)
							pCamera->Set_isShake(true, 0.3f, 3);

						m_pSound_Device->SoundStop(16);
						m_pSound_Device->SoundPlay(14, 0);
						m_pSound_Device->SoundPlay(15, 0);
					}
				}
			}
		}
	}
}

_int CDemonLord::Update_GameObject(const _float & fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;
	Update_Collider();
	Collider_Att();

	if (false == m_isEnter)
	{
		if (nullptr == m_pMeshCom
			|| nullptr == m_pObject_Manager)
			return _RUN;

		CCursor*	pCursor = (CCursor*)m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Cursor");
		if (nullptr == pCursor)
			return _RUN;
		pCursor->Set_isSee(false);

		if (Ani_Cinematic_Landing == m_pMeshCom->Get_CurrentAniIdx())
		{
			if(true == m_pMeshCom->is_Finished())
				Set_Animation(Ani_Cinematic_Cutthroat);
		}
		else if (Ani_Cinematic_Cutthroat == m_pMeshCom->Get_CurrentAniIdx())
		{
			if (true == m_pMeshCom->is_Finished())
			{
				Set_Animation(Ani_Idle);
				m_isEnter = true;
				const CUnit*		pPlayer = dynamic_cast<CUnit*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"));
				CCamera_Follow*	pCamera = dynamic_cast<CCamera_Follow*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Camera"));

				if (nullptr != pCamera
					&& nullptr != pPlayer)
				{
					pCamera->Set_MouseRot(true);
					pCamera->Set_MouseMove(true);
					pCamera->Set_LookDirction(false);
					pCamera->Set_Target(pPlayer, _vec3(0.f, 7.f, 3.f), 15.f);
				}

				CCursor*	pCursor = dynamic_cast<CCursor*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Cursor"));
				if (nullptr == pCursor)
					return _RUN;
				pCursor->Set_isSee(true);

				m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &m_vTargetPos);

				//m_pSound_Device->SoundPlay(4, 0);
			}
		}
		else
		{
			if (nullptr == m_pTransformCom
				&& nullptr == m_pObject_Manager)
				return _RUN;
			
			_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
			_vec3	vDir = m_vTargetPos - vPos;
			if (1.f > D3DXVec3Length(&vDir))
			{
				m_pSound_Device->SoundStop(5);
				m_pSound_Device->SoundPlay(3, 0);
				//플레이어 바라보기
				const CTransform* pPlayerTransform = dynamic_cast<const CTransform*>(m_pObject_Manager->Get_Component(SCENE_STAGE, L"Layer_Player", L"Com_Transform"));
				_vec3	vPlayerPos = *(pPlayerTransform->Get_Infomation(CTransform::INFO_POSITION));
				Set_Direction(vPlayerPos - vPos, true);
				Set_Animation(Ani_Cinematic_Landing);
			}
			else
			{
				m_pTransformCom->Go_Direction(vDir, fTimeDelta * SPEED);
				Set_Direction(vDir, false, 1.f);
			}
		}

		//Skip
		if (CInput_Device::Get_Instance()->Get_KeyState(DIK_RETURN) & 0x80)
		{
			Set_Animation(Ani_Idle);
			m_isEnter = true;
			const CUnit*		pPlayer = dynamic_cast<CUnit*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"));
			CCamera_Follow*	pCamera = dynamic_cast<CCamera_Follow*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Camera"));

			if (nullptr != pCamera
				&& nullptr != pPlayer)
			{
				pCamera->Set_MouseRot(true);
				pCamera->Set_MouseMove(true);
				pCamera->Set_LookDirction(false);
				pCamera->Set_Target(pPlayer, _vec3(0.f, 7.f, 3.f), 15.f);
			}

			CCursor*	pCursor = dynamic_cast<CCursor*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Cursor"));
			if (nullptr == pCursor)
				return _RUN;
			pCursor->Set_isSee(true);

			m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &m_vTargetPos);

			//m_pSound_Device->SoundPlay(4, 0);
		}
	}

	else
	{
		if (m_tUnitInfo.fHp / m_tUnitInfo.fMaxHp <= 0.3f)
		{
			Skill_3(fTimeDelta);

			if (true == m_isSkill3)
				Att_Basic(fTimeDelta);
		}
		else if (m_tUnitInfo.fHp / m_tUnitInfo.fMaxHp <= 0.5f)
		{
			Skill_2(fTimeDelta);

			if (true == m_isSkill2)
				Att_Basic(fTimeDelta);
		}
		else if (m_tUnitInfo.fHp / m_tUnitInfo.fMaxHp <= 0.7f)
		{
			Skill_1(fTimeDelta);

			if(true == m_isSkill1)
				Att_Basic(fTimeDelta);
		}
		else
		{
			Att_Basic(fTimeDelta);
		}
	}

	if (nullptr == m_pMeshCom)
		return _RUN;

	_uint	iAni = m_pMeshCom->Get_CurrentAniIdx();
	if (Ani_Hurt ==iAni
		|| Ani_Attack_Pound ==iAni
		|| Ani_Shock_Ground ==iAni
		|| Ani_Fly_Pound_End ==iAni)
	{
		if (true == m_pMeshCom->is_Finished())
		{
			Set_Animation(Ani_Idle);
		}
	}

	if (true == m_isDeadAni)
	{
		Set_Animation(Ani_Death);
		if (true == m_pMeshCom->is_Finished()
			&& nullptr != m_pHpBar
			&& nullptr != m_pHpBarBG
			&& nullptr != m_pHpBarFR)
		{
			m_pHpBar->Set_isDead(true);
			m_pHpBarBG->Set_isDead(true);
			m_pHpBarFR->Set_isDead(true);

			Set_isDead(true);
		}
	}

	return _RUN;
}

_int CDemonLord::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UNIT, this);

	return _int();
}

void CDemonLord::Render_GameObject(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pShaderCom
		|| nullptr == m_pMeshCom)
		return;

	if(true == m_isAniPlay)
		m_pMeshCom->Play_Animation(m_fTimeDelta * 0.5f * m_vecAniInfo[m_pMeshCom->Get_CurrentAniIdx()]->fSpeed);

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		_MSG_BOX("CDemonLord SetUp_ConstantTable Failed");
		return;
	}

	if (FAILED(pEffect->Begin(nullptr, 0)))
		int a = 0;

	m_pMeshCom->Render_Mesh(pEffect, 0);

	pEffect->End();	

	m_pColliderCom->Render_Collider();
	m_pLeftColliderCom->Render_Collider();
	m_pRightColliderCom->Render_Collider();
}

CDemonLord * CDemonLord::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDemonLord* pInstance = new CDemonLord(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CDemonLord Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDemonLord::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CDemonLord(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CDemonLord Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDemonLord::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CDemonLord(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CDemonLord Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDemonLord::Free(void)
{
	if (false == m_isClone)
	{
		for (auto iter : m_vecAniInfo)
			Safe_Delete(iter);
		m_vecAniInfo.clear();
	}

	if (nullptr != m_pTargetUnit)
		Safe_Release(m_pTargetUnit);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNaviCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pLeftColliderCom);
	Safe_Release(m_pRightColliderCom);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pSound_Device);

	Safe_Release(m_pHpBar);
	Safe_Release(m_pHpBarFR);
	Safe_Release(m_pHpBarBG);

	CGameObject::Free();
}