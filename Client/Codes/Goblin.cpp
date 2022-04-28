#include "stdafx.h"
#include "Defines.h"
#include "Goblin.h"

#include "Renderer.h"
#include "Mesh_Dynamic.h"
#include "Shader.h"
#include "Transform.h"
#include "Navigation.h"
#include "Collider.h"
#include "AStar.h"
#include "Frustum_Culling.h"

#include "HpBarBG_Enemy.h"
#include "HpBar_Enemy.h"
#include "Font_Damage.h"

#include "Input_Device.h"
#include "Object_Manager.h"
#include "Sound_Device.h"
#include "Turn.h"

const _float KNOCK_BACK_SPEED = 10.f;

CGoblin::CGoblin(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUnit(pGraphic_Device)
{
}

CGoblin::CGoblin(const CGoblin & rhs)
	: CUnit(rhs)
	, m_isJump(false)
	, m_matRotY(rhs.m_matRotY)
	, m_iCurrentIdx(rhs.m_iCurrentIdx)
	, m_isPlayAni(rhs.m_isPlayAni)
	, m_vecAniInfo(rhs.m_vecAniInfo)
	, m_iTest(0)
	, m_vCrystalPos(rhs.m_vCrystalPos)
	, m_isAtt(false)
	, m_isAttOnce(false)
	, m_vecStairInfo(rhs.m_vecStairInfo)
	, m_vTargetPos(rhs.m_vTargetPos)
{
}

const _matrix * CGoblin::Get_WorldMatrix(void) const
{
	if (nullptr == m_pTransformCom)
	{
		_matrix	matIden;
		return D3DXMatrixIdentity(&matIden);
	}
	return m_pTransformCom->Get_WorldMatrix();
}

const _matrix * CGoblin::Get_matRotY(void) const
{
	return &m_matRotY;
}

void CGoblin::Set_Target(CUnit * pTarget)
{
	//부딪혀있으면 세팅 취소
	if (true == m_pColliderCom->Get_isCollision())
		return;

	if (nullptr != m_pTargetUnit)
		Safe_Release(m_pTargetUnit);

	m_pTargetUnit = pTarget;
	m_pTargetUnit->AddRef();

	m_vTargetPos = *((CTransform*)pTarget->Get_Component(L"Com_Transform"))->Get_Infomation(CTransform::INFO_POSITION);
	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	_vec3	vDir = vPos - m_vTargetPos;
	_float	fDistance = D3DXVec3Length(&vDir);

	if (1.f > fDistance)
	{
		m_isAtt = true;
		Set_Direction(vDir);
		m_listBest.clear();
	}
	else
	{
		m_listBest = m_pAStarCom->Make_Rute(m_iCurrentIdx, *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), m_vTargetPos);
		m_isFollower = true;
	}
}

void CGoblin::Add_Hp(const _float & fHpAmount)
{
	CUnit::Add_Hp(fHpAmount);

	if (false == m_isDeadAni)
	{
		if (fHpAmount < 0)
		{
			Set_Animation(Ani_Hurt);

			_float	fDamege = fHpAmount + m_tUnitInfo.fDef;
			m_tUnitInfo.fHp += fDamege;

			if (nullptr != m_pObject_Manager
				&& nullptr != m_pTransformCom)
			{
				//Blood Effect
				m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Blood", L"Object_Blood"
					, m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION));

				//Damage Font
				_vec3	vFontPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

				m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Font_Damage", L"Object_Font_Damage", &vFontPos);

				CFont_Damage* pFont = dynamic_cast<CFont_Damage*>(m_pObject_Manager
					->Get_LastGameObject(SCENE_STAGE, L"Layer_Font_Damage"));

				if (nullptr != pFont)
					pFont->Set_Font((_uint)(fDamege * -1.f));

				if (nullptr != m_pSound_Device)
					m_pSound_Device->SoundPlay(19, 0);
			}
		}
	}

	if (0.f >= m_tUnitInfo.fHp)
	{
		m_pColliderCom->Set_isTargetDead(true);
		m_pClubColliderCom->Set_isTargetDead(true);
		m_isDeadAni = true;
	}
}

void CGoblin::Set_Knock_Back(void)
{
	m_isKnockBack = true;
}

void CGoblin::Knock_Back(const _float & fTimeDelta)
{
	Set_Animation(Ani_Hurt);

	//Go to back direction
	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	_vec3	vBack = (*m_pTransformCom->Get_Infomation(CTransform::INFO_LOOK))/* * -1.f*/;

	m_pTransformCom->Go_Direction(vBack, fTimeDelta * KNOCK_BACK_SPEED);

	//If the next position isn't in the nevigation mesh
	_vec3	vNextPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	m_iCurrentIdx = m_pNaviCom->Compute_CurrentIndex(vNextPos, &m_iCurrentIdx);
	vNextPos.y = m_pNaviCom->Get_Hight(vNextPos, &m_iCurrentIdx);
	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &vNextPos);

	_uint	iResult = m_pNaviCom->Check_Exist(vNextPos, m_iCurrentIdx);

	if (CNavigation::EXIST_OUT == iResult)
	{
		//Go to before position
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &vPos);
		m_iCurrentIdx = m_pNaviCom->Compute_CurrentIndex(vPos, &m_iCurrentIdx);
	}

	//If the hurt animation is end, Remove the Knock back
	if (true == m_pMeshCom->is_Finished())
	{
		Set_Animation(Ani_Idle);
		m_isKnockBack = false;

		Compute_TargetPos();
	}
}

HRESULT CGoblin::Ready_GameObject_Prototype(void)
{
	Init_UnitInfo(100.f, 50.f, 20.f);

	m_vCrystalPos = _vec3(85.0352554f, 0.f, 115.778961f);

	if (FAILED(Ready_Component())
		|| FAILED(Ready_AniInfo()))
		return E_FAIL;

	HANDLE	hFile;

	hFile = CreateFile(L"../Bin/Data/PlayerPos.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	_ulong dwByte = 0;

	for (;;)
	{
		_vec3	pPos;

		ReadFile(hFile, &pPos, sizeof(_vec3), &dwByte, 0);
		if (dwByte == 0)
			break;
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(pPos.x - 2.f, pPos.y, pPos.z - 2.f));
	}

	CloseHandle(hFile);

	m_vecStairInfo.reserve(8);

	hFile = CreateFile(L"../Bin/Data/UpStairPos.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	for (;;)
	{
		_vec3	vPos;

		ReadFile(hFile, &vPos, sizeof(_vec3), &dwByte, 0);
		if (dwByte == 0)
			break;

		STAIRINFO*	pStairInfo = new STAIRINFO;
		ZeroMemory(pStairInfo, sizeof(STAIRINFO));

		pStairInfo->vUpStairPos = vPos;
		m_vecStairInfo.push_back(pStairInfo);
	}

	CloseHandle(hFile);

	hFile = CreateFile(L"../Bin/Data/DownStairPos.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	_uint i = 0;
	for (;;)
	{
		_vec3	vPos;

		ReadFile(hFile, &vPos, sizeof(_vec3), &dwByte, 0);
		if (dwByte == 0)
			break;

		m_vecStairInfo[i]->vDownStairPos = vPos;
		++i;
	}

	CloseHandle(hFile);

	m_vecStairInfo[7]->vUpStairPos.y = m_vecStairInfo[6]->vUpStairPos.y;

	m_pTransformCom->Scaling(0.08f, 0.08f, 0.08f);
	D3DXMatrixIdentity(&m_matRotY);

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	SetUp_LightAndMaterialInfo_OnShader(pEffect);

	return NOERROR;
}

HRESULT CGoblin::Ready_GameObject_Clone(const _vec3* pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pMeshCom = (Engine::CMesh_Dynamic*)Get_Component(L"Com_Mesh");
	m_pMeshCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	m_pNaviCom = (Engine::CNavigation*)Get_Component(L"Com_Navigation");
	m_pNaviCom->AddRef();

	m_pColliderCom = (Engine::CCollider*)Get_Component(L"Com_Collider");
	m_pColliderCom->AddRef();

	m_pClubColliderCom = (Engine::CCollider*)Get_Component(L"Com_ClubCollider");
	m_pClubColliderCom->AddRef();

	m_pAStarCom = (Engine::CAStar*)Get_Component(L"Com_AStar");
	m_pAStarCom->AddRef();

	m_pCullingCom = (Engine::CFrustum_Culling*)Get_Component(L"Com_Culling");
	m_pCullingCom->AddRef();

	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pObject_Manager->AddRef();

	m_pSound_Device = CSound_Device::Get_Instance();
	m_pSound_Device->AddRef();

	if(nullptr != pPos)
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, pPos);

	Set_Animation(Ani_Idle);
 	m_pNaviCom->Compute_CurrentIndex(*m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), &m_iCurrentIdx);

	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	_vec3	vMin, vMax;

	//로컬 좌표
	vMin = _vec3(-8.f, 0.f, -8.f);
	vMax = _vec3(8.f, 35.f, 8.f);

	m_pColliderCom->Set_Collider(CCollider::TYPE_AABB, &vMin, &vMax, m_pTransformCom);

	vMin = _vec3(-2.f, -1.f, -2.f);
	vMax = _vec3(2.f, 12.f, 2.f);
	m_pClubColliderCom->Set_Collider(CCollider::TYPE_OBB, &vMin, &vMax, m_pTransformCom);

	Compute_TargetPos();

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

HRESULT CGoblin::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	return NOERROR;
}

HRESULT CGoblin::Ready_Component(void)
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
	m_pMeshCom = (Engine::CMesh_Dynamic*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Mesh_Goblin");
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

	m_pClubColliderCom = (Engine::CCollider*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Collider");
	if (nullptr == m_pClubColliderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_ClubCollider", m_pClubColliderCom);

	// For.Com_Collider
	m_pAStarCom = (Engine::CAStar*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_AStar");
	if (nullptr == m_pAStarCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_AStar", m_pAStarCom);

	// For.Com_Culling
	m_pCullingCom = (Engine::CFrustum_Culling*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Frustum_Culling");
	if (nullptr == m_pCullingCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Culling", m_pCullingCom);

	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pObject_Manager->AddRef();

	m_pSound_Device = CSound_Device::Get_Instance();
	m_pSound_Device->AddRef();

	return NOERROR;
}

HRESULT CGoblin::Ready_AniInfo(void)
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
	pAniInfo->iState = Ani_Shok;
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
	pAniInfo->iState = Ani_Jump;
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
	pAniInfo->iState = Ani_Cough;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Attack2;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Attack1;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	return NOERROR;
}

HRESULT CGoblin::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

void CGoblin::Set_Animation(const eANINAME & eAniName)
{
	m_pMeshCom->SetUp_AnimationSet(eAniName, m_vecAniInfo[eAniName]->fWeight);
	m_pMeshCom->Set_AdjustPeriod(m_vecAniInfo[eAniName]->dPeriode);
}

void CGoblin::Set_Direction(const _vec3 & vDir)
{
	_vec3	vNormalLook;
	D3DXVec3Normalize(&vNormalLook, m_pTransformCom->Get_Infomation(CTransform::INFO_LOOK));
	_vec3	vDir_New;
	D3DXVec3Normalize(&vDir_New, &vDir);
	_float	fCos = D3DXVec3Dot(&vNormalLook, &vDir_New);
	fCos = acosf(-fCos);

	if (false == isnan(fCos) && 1 < fCos)
		m_pTransformCom->RotationY(fCos);

}

void CGoblin::Update_Collider(void)
{
	if (true == m_pObject_Manager->Check_Collision_AABB(SCENE_STAGE, m_pColliderCom, L"Layer_Player") ||
		true == m_pObject_Manager->Check_Collision_AABB(SCENE_STAGE, m_pColliderCom, L"Layer_Mesh_Core_Crystal_0") ||
		true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pColliderCom, L"Layer_SpikyTower") ||
		true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pColliderCom, L"Layer_SpikyTower", nullptr, L"Com_AttCollider"))
		m_isAtt = true;

	if (true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pClubColliderCom, L"Layer_Player") ||
		true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pClubColliderCom, L"Layer_Mesh_Core_Crystal_0") ||
		true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pClubColliderCom, L"Layer_SpikyTower") ||
		true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pClubColliderCom, L"Layer_SpikyTower", nullptr, L"Com_AttCollider"))
		m_isAtt = true;

	m_pObject_Manager->Check_Collision_AABB(SCENE_STAGE, m_pColliderCom, L"Layer_Goblin");

	_matrix matParent = *m_pMeshCom->Get_FrameCombinedMatrix("b_HandR") * *m_pTransformCom->Get_WorldMatrix();

	_vec3	vCenter = *(_vec3*)matParent.m[3];

	_matrix	matWorld, matY, matZ;
	D3DXMatrixIdentity(&matWorld);

	D3DXMatrixRotationX(&matY, -0.3f);
	D3DXMatrixRotationZ(&matZ, 109.3f);

	matWorld *= matY * matZ * matParent;
	memcpy(&matWorld.m[3], &vCenter, sizeof(_vec3));

	m_pClubColliderCom->Set_CollisionWorld(&matWorld);

	m_pColliderCom->Update_Collider();
	m_pClubColliderCom->Update_Collider();
}

void CGoblin::Collider_Att(const _float& fTimeDelta)
{
	if (true == m_isAttOnce)
	{
		m_fAccAtt += fTimeDelta;
		//if (m_fAccAtt > 0.3f)
		{
			CUnit*	pTmp = nullptr;
			if (nullptr != m_pTargetUnit)
				pTmp = m_pTargetUnit;

			if (true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pClubColliderCom, L"Layer_Player", (CGameObject**)&m_pTargetUnit)
				|| true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pClubColliderCom, L"Layer_Mesh_Core_Crystal_0", (CGameObject**)&m_pTargetUnit)
				|| true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pClubColliderCom, L"Layer_SpikyTower", (CGameObject**)&m_pTargetUnit)
				|| true == m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pClubColliderCom, L"Layer_SpikyTower", (CGameObject**)&m_pTargetUnit), L"Com_AttCollider")
			{
				if (nullptr == m_pTargetUnit)
					return;

				if (nullptr != m_pTargetUnit)
				{
					m_pTargetUnit->Add_Hp(-m_tUnitInfo.fAtt);
				}

				_vec3	vDir = *((CTransform*)m_pTargetUnit->Get_Component(L"Com_Transform"))->Get_Infomation(CTransform::INFO_POSITION) - *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
				Set_Direction(vDir);

				Safe_Release(pTmp);
				m_pTargetUnit->AddRef();
				m_isAttOnce = false;
			}
		}
	}

	else
	{
		m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pClubColliderCom, L"Layer_Player");
		m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pClubColliderCom, L"Layer_SpikyTower");
		m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pClubColliderCom, L"Layer_SpikyTower", nullptr, L"Com_AttCollider");
		m_pObject_Manager->Check_Collision_OBB(SCENE_STAGE, m_pClubColliderCom, L"Layer_Mesh_Core_Crystal_0");
	}
}

void CGoblin::Compute_TargetPos(void)
{
	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

	for (auto& pInfo : m_vecStairInfo)
	{
		pInfo->fUnitDistance = D3DXVec3Length(&(pInfo->vDownStairPos - vPos));
		pInfo->fCrystalDistance = D3DXVec3Length(&(m_vCrystalPos - pInfo->vUpStairPos));
		pInfo->fHightGap = abs(pInfo->vUpStairPos.y - vPos.y);
		pInfo->fTotalCost = pInfo->fUnitDistance + pInfo->fCrystalDistance + pInfo->fHightGap;
	}

	_uint	iBestIdx = 0;
	if (m_vCrystalPos.y - vPos.y > 0.9f)
	{
		sort(m_vecStairInfo.begin(), m_vecStairInfo.end(), [=](STAIRINFO* pSour, STAIRINFO* pDest) {return pSour->fTotalCost < pDest->fTotalCost; });

		_float	fHightGap = 0.f;
		_uint	iStairSize = m_vecStairInfo.size();
		for (_uint i = 0; i < iStairSize; ++i)
		{
			if (m_vecStairInfo[i]->fHightGap > 0.2)
			{
				if (0.f == fHightGap)
				{
					fHightGap = m_vecStairInfo[i]->fHightGap;
					iBestIdx = i;
				}
				else
				{
					if (fHightGap > m_vecStairInfo[i]->fHightGap)
					{
						fHightGap = m_vecStairInfo[i]->fHightGap;
						iBestIdx = i;
					}
				}
			}
		}

		m_vTargetPos = m_vecStairInfo[iBestIdx]->vDownStairPos;
	}
	else
		m_vTargetPos = m_vCrystalPos;

	m_pNaviCom->Compute_CurrentIndex(vPos, &m_iCurrentIdx);
	m_listBest = m_pAStarCom->Make_Rute(m_iCurrentIdx, *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), m_vTargetPos);
	if(0 < m_listBest.size())
		m_listBest.push_back(m_vecStairInfo[iBestIdx]->vUpStairPos);
}

_int CGoblin::Update_GameObject(const _float & fTimeDelta)
{
	if (true == m_isDeadAni)
	{
		Set_Animation(Ani_Death);
		if (true == m_pMeshCom->is_Finished())
		{
			m_isDead = true;
			//m_pColliderCom->Set_isDead(true);
			m_pClubColliderCom->Set_isDead(true);
			m_pHpBar->Set_isDead(true);
			m_pHpBarBG->Set_isDead(true);
		}
	}
	else
	{
		m_fTimeDelta = fTimeDelta;

		Update_Collider();
		Collider_Att(fTimeDelta);

		_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

		//넉백
		if (true == m_isKnockBack)
		{
			Knock_Back(fTimeDelta);
		}

		if (false == m_isAtt)
		{
			
			{
				//이동
				if (m_listBest.size() > 0)
				{
					_vec3	vDir = *(m_listBest.begin()) - vPos;

					Set_Animation(Ani_MoveForward);
					m_pTransformCom->Go_Direction(vDir, 5.f * fTimeDelta);

					Update_Collider();
					if (true == m_pColliderCom->Get_isCollision() || true == m_pClubColliderCom->Get_isCollision())
					{
						Set_Animation(Ani_MoveForward);
						m_pTransformCom->Go_Direction(vDir, -5.f * fTimeDelta);
					}

					if (D3DXVec3Length(&vDir) < 0.5f)
						m_listBest.pop_front();

					//방향
					Set_Direction(vDir);

				}
				else
				{
					Update_Collider();
					if (false == m_isFollower)
						Compute_TargetPos();
					else
					{
						m_vTargetPos = *((CTransform*)m_pTargetUnit->Get_Component(L"Com_Transform"))->Get_Infomation(CTransform::INFO_POSITION);
						_vec3	vCurrentPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
						m_pNaviCom->Compute_CurrentIndex(vCurrentPos, &m_iCurrentIdx);
						m_listBest = m_pAStarCom->Make_Rute(m_iCurrentIdx, vCurrentPos, m_vTargetPos);
					}
				}
			}
		}
		else
			Update_Collider();

		if (true == m_isAtt)
		{
			//한번만 들어오도록
			if (false == m_isAttMotion)
			{
				m_isAttMotion = true;
				m_isAttOnce = true;
				m_fAccAtt = 0.f;
			}

			if (Ani_Hurt != m_pMeshCom->Get_CurrentAniIdx())
				Set_Animation(Ani_Attack1);

			if (Ani_Attack1 == m_pMeshCom->Get_CurrentAniIdx() && true == m_pMeshCom->is_Finished())
			{
				Set_Animation(Ani_Idle);
				m_isAtt = false;
				m_isAttMotion = false;
				m_isAttOnce = false;
			}
		}

	}

	if (true == m_pMeshCom->is_Finished())
		Set_Animation(Ani_Idle);

	return _RUN;
}

_int CGoblin::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (true == m_pCullingCom->isIn_Frustum(m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), 2.f))
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UNIT, this);

	return _int();
}

void CGoblin::Render_GameObject(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pShaderCom
		|| nullptr == m_pMeshCom)
		return;

	m_pMeshCom->Play_Animation(m_fTimeDelta * 0.5f);

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		_MSG_BOX("CGoblin SetUp_ConstantTable Failed");
		return;
	}

	if (FAILED(pEffect->Begin(nullptr, 0)))
		int a = 0;

	m_pMeshCom->Render_Mesh(pEffect, 0);

	pEffect->End();	

	m_pColliderCom->Render_Collider();
	m_pClubColliderCom->Render_Collider();
}

CGoblin * CGoblin::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGoblin* pInstance = new CGoblin(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CGoblin Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGoblin::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CGoblin(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CGoblin Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGoblin::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CGoblin(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CGoblin Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGoblin::Free(void)
{
	if (false == m_isClone)
	{
		for (auto iter : m_vecAniInfo)
			Safe_Delete(iter);
		m_vecAniInfo.clear();

		for (auto& pInfo : m_vecStairInfo)
			Safe_Delete(pInfo);
		m_vecStairInfo.clear();
	}

	//if (nullptr != m_pTargetUnit)
	//	Safe_Release(m_pTargetUnit);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNaviCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pClubColliderCom);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pSound_Device);
	Safe_Release(m_pHpBar);
	Safe_Release(m_pHpBarBG);

	CGameObject::Free();
}
