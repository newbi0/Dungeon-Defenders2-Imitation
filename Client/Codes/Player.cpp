#include "stdafx.h"
#include "Defines.h"
#include "Player.h"

//Component
#include "Renderer.h"
#include "Mesh_Dynamic.h"
#include "Shader.h"
#include "Transform.h"
#include "Navigation.h"
#include "Collider.h"
#include "KeyChecker.h"

//SingleTone
#include "Object_Manager.h"
#include "Sound_Device.h"
#include "Input_Device.h"

//Att
#include "Weapon.h"
#include "SpikyTower.h"

//UI
#include "HpBar_Player.h"
#include "Font_Damage.h"
#include "Inventory.h"
#include "Profile.h"
#include "Cursor.h"

//Object
#include "ItemIcon.h"

//Effect
#include "PowerUpAura.h"

//Utility
#include "Turn.h"
#include "Camera_Follow.h"

const float SPEED = 6.f;
const float JUMP_AMOUNT = 1.f;
const float JUMP_END_POINT = 0.2f;
const float JUMP_UP_SPEED = 1.5f;
const float JUMP_DOWN_SPEED = 2.f;
const float PULL_SPEED = 10.f;
const float PULL_END_POINT = 2.f;
const float ITEM_END_DISTANCE = 3.f;
const float DAMEGE_HIGHT = 3.f;
const float SKILL0_PER_TIME = 0.5f;
const float SKILL0_PER_HEAL_AMOUNT = 10.f;
const unsigned int SKILL0_MAXCNT = 5;
const unsigned int SKILL0_EFFECT_MAXCNT= 5;
const unsigned int SKILL0_EFFECT_RAND_VALUE = 60;
const float SKILL1_READY_TIME = 0.5f;
const float SKILL2_READY_TIME = 0.1f;
const float SKILL2_CONTINUE_TIME = 5.f;
const float SKILL2_BUFF_MULTI_VALUE = 2.f;

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUnit(pGraphic_Device)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CUnit(rhs)
	, m_matRotY(rhs.m_matRotY)
	, m_iCurrentIdx(rhs.m_iCurrentIdx)
	, m_isPlayAni(rhs.m_isPlayAni)
	, m_vecAniInfo(rhs.m_vecAniInfo)
{
}

const _matrix * CPlayer::Get_WorldMatrix(void) const
{
	if (nullptr == m_pTransformCom)
	{
		_matrix	matIden;
		return D3DXMatrixIdentity(&matIden);
	}

	return m_pTransformCom->Get_WorldMatrix();
}

const _matrix * CPlayer::Get_matRotY(void) const
{
	return &m_matRotY;
}

void CPlayer::Set_Weapon(CWeapon * pWeapon)
{
	if (nullptr == pWeapon)
		return;
	m_pWeapon = pWeapon;
	m_pWeapon->AddRef();
}

HRESULT CPlayer::Ready_GameObject_Prototype(void)
{
	//Basic Info Set
	Init_UnitInfo(100.f, 50.f, 40.f);

	if (FAILED(Ready_Component())
		|| FAILED(Ready_AniInfo()))
		return E_FAIL;

	//Init Position
	HANDLE	hFile;

	hFile = CreateFile(L"../Bin/Data/PlayerPos.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	_ulong dwByte = 0;

	for (;;)
	{
		_vec3	pPos;

		ReadFile(hFile, &pPos, sizeof(_vec3), &dwByte, 0);
		if (dwByte == 0)
			break;
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &pPos);
	}

	CloseHandle(hFile);

	m_pTransformCom->Scaling(0.08f, 0.08f, 0.08f);
	D3DXMatrixIdentity(&m_matRotY);

	//Light Info Set
	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	SetUp_LightAndMaterialInfo_OnShader(pEffect);

	return NOERROR;
}

HRESULT CPlayer::Ready_GameObject_Clone(const _vec3* pPos)
{
	m_pRendererCom = dynamic_cast<Engine::CRenderer*>(Get_Component(L"Com_Renderer"));
	if (nullptr == m_pRendererCom)
		return E_FAIL;
	m_pRendererCom->AddRef();

	m_pTransformCom = dynamic_cast<Engine::CTransform*>(Get_Component(L"Com_Transform"));
	if (nullptr == m_pTransformCom)
		return E_FAIL;
	m_pTransformCom->AddRef();

	m_pMeshCom = dynamic_cast<Engine::CMesh_Dynamic*>(Get_Component(L"Com_Mesh"));
	if (nullptr == m_pMeshCom)
		return E_FAIL;
	m_pMeshCom->AddRef();

	m_pShaderCom = dynamic_cast<Engine::CShader*>(Get_Component(L"Com_Shader"));
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	m_pShaderCom->AddRef();

	m_pNaviCom = dynamic_cast<Engine::CNavigation*>(Get_Component(L"Com_Navigation"));
	if (nullptr == m_pNaviCom)
		return E_FAIL;
	m_pNaviCom->AddRef();

	m_pColliderCom = dynamic_cast<Engine::CCollider*>(Get_Component(L"Com_Collider"));
	if (nullptr == m_pColliderCom)
		return E_FAIL;
	m_pColliderCom->AddRef();

	m_pKeyCheckerCom = dynamic_cast<Engine::CKeyChecker*>(Get_Component(L"Com_KeyChecker"));
	if (nullptr == m_pKeyCheckerCom)
		return E_FAIL;
	m_pKeyCheckerCom->AddRef();

	m_pObject_Manager = CObject_Manager::Get_Instance();
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	m_pObject_Manager->AddRef();

	m_pSound_Device = CSound_Device::Get_Instance();
	if (nullptr == m_pSound_Device)
		return E_FAIL;
	m_pSound_Device->AddRef();

	m_pInventory = dynamic_cast<CInventory*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Inventory"));
	if (nullptr == m_pInventory)
		return E_FAIL;

	m_pProfile = dynamic_cast<CEquipmentBox*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Profile"));
	if (nullptr == m_pProfile)
		return E_FAIL;

	m_pCursor = dynamic_cast<CCursor*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Cursor"));
	if (nullptr == m_pCursor)
		return E_FAIL;

	m_pCamera = dynamic_cast<CCamera_Follow*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Camera"));
	if (nullptr == m_pCamera)
		return E_FAIL;

	if (nullptr != pPos)
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, pPos);

	Set_Animation(Ani_Idle);

	//Init Idx Set
 	m_pNaviCom->Compute_CurrentIndex(*m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), &m_iCurrentIdx);

	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

	//Init Collider
	_vec3	vMin, vMax;

	vMin = _vec3(-8.f, 0.f, -8.f);
	vMax = _vec3(8.f, 35.f, 8.f);

	m_pColliderCom->Set_Collider(CCollider::TYPE_AABB, &vMin, &vMax, m_pTransformCom);

	//Init HpBar
	CHpBar_Player*	pHpBar = dynamic_cast<CHpBar_Player*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_HpBar_Player_Left"));
	if (nullptr == pHpBar)
		return E_FAIL;

	pHpBar->Set_UnitHpBar(this);

	return NOERROR;
}

HRESULT CPlayer::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	return NOERROR;
}

HRESULT CPlayer::Ready_Component(void)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	// For.Com_Renderer
	m_pRendererCom = dynamic_cast<Engine::CRenderer*>
		(m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Renderer"));
	if (nullptr == m_pRendererCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Renderer", m_pRendererCom);

	// For.Com_Transform	
	m_pTransformCom = dynamic_cast<Engine::CTransform*>(m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Transform"));
	if (nullptr == m_pTransformCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Transform", m_pTransformCom);

	// For.Com_Navigation
	m_pNaviCom = dynamic_cast<Engine::CNavigation*>(m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Navigation"));
	if (nullptr == m_pNaviCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Navigation", m_pNaviCom);

	// For.Com_Mesh
	m_pMeshCom = dynamic_cast<Engine::CMesh_Dynamic*>(m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Mesh_Player"));
	if (nullptr == m_pMeshCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Mesh", m_pMeshCom);
	
	// For.Com_Shader
	m_pShaderCom = dynamic_cast<Engine::CShader*>(m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Shader_Unit"));
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	// For.Com_Collider
	m_pColliderCom = dynamic_cast<Engine::CCollider*>(m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Collider"));
	if (nullptr == m_pColliderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Collider", m_pColliderCom);

	// For.Com_KeyChecker
	m_pKeyCheckerCom = dynamic_cast<Engine::CKeyChecker*>(m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_KeyChecker"));
	if (nullptr == m_pKeyCheckerCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_KeyChecker", m_pKeyCheckerCom);

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

HRESULT CPlayer::Ready_AniInfo(void)
{
	m_vecAniInfo.reserve(Ani_END);

	ANIINFO*	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_WaveWave;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Win;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_WaveStart;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Upgrade;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_TurnLeft;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_SummonStop;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Summon_Place;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Summon;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Spawn;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_RunForward;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Repair;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_PickupItem;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Move_Right;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Move_Left;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Move_BackWard;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_MeleeAttack3_medium;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_MeleeAttack2_medium;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_MeleeAttack1_medium;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Lose;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_LevelUp;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Jump_Falling;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Jump;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.3;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Idle_LowHealth;
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
	pAniInfo->iState = Ani_Heal;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Death;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Detonate;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Circluarslice;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.1;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_CallOut;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Blocking_Large_Finch;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_Blocking_Large;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	pAniInfo = new ANIINFO;
	pAniInfo->iState = Ani_ActivateBloodRage;
	pAniInfo->isRepetable = false;
	pAniInfo->dPeriode = 0.0;
	pAniInfo->fSpeed = 1.f;
	pAniInfo->fWeight = 0.1f;
	m_vecAniInfo.push_back(pAniInfo);

	return NOERROR;
}

HRESULT CPlayer::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

void CPlayer::Set_Animation(const ANINAME & eAniName)
{
	if (nullptr == m_pMeshCom
		|| Ani_END == eAniName)
		return;

	m_pMeshCom->SetUp_AnimationSet(eAniName, m_vecAniInfo[eAniName]->fWeight);
	m_pMeshCom->Set_AdjustPeriod(m_vecAniInfo[eAniName]->dPeriode);
}

void CPlayer::Set_isBuild(_bool isBuildOn)
{
	if (nullptr == m_pCursor
		|| nullptr == m_pCamera)
		return;

	m_isBuildOn = isBuildOn;

	if (false == m_isBuildOn)
	{
		m_pCursor->Set_isFixity(true);
		m_pCamera->Set_MouseMove(true);
	}
}

void CPlayer::Set_PullOut(const _bool& isPulledBack, const _vec3 & vPulledPos, const _float& fPulledSpeed)
{
	m_isPulledBack = isPulledBack;
	m_vPulledPos = vPulledPos;
	m_fPulledSpeed = fPulledSpeed;
}

void CPlayer::InputProcess(const _float& fTimeDelta)
{
	if (nullptr == m_pSound_Device
		|| nullptr == m_pKeyCheckerCom
		|| nullptr == m_pInput_Device)
		return;

	if (false == m_isJump
		&& true == m_pMeshCom->is_Finished()
		&& false == m_isAtt
		&& false == m_isDeadAni
		&& false == m_isSkill_1)
		Set_Animation(Ani_Idle);

	if (false == m_pInventory->Get_SeeAble()
		&& false == m_pProfile->Get_SeeAble()
		&& false == m_isBuildOn)
	{
		if (false == m_isAtt
			&& false == m_isSkill_1)
			Move(fTimeDelta, SPEED);

		Jump(fTimeDelta);

		Basic_Att(fTimeDelta);

		//Skill
		if (m_pInput_Device->Get_KeyState(DIK_0) & 0x80)
		{
			m_pCamera->Set_isShake(true, 0.3f, 3, 8.f);
		}

		//Skill
		if (m_pInput_Device->Get_KeyState(DIK_1) & 0x80)
		{
			m_pSound_Device->SoundPlay(21, 0);
			Skill_0(fTimeDelta);
			m_isSkill_0 = true;
		}


		if (m_pInput_Device->Get_KeyState(DIK_2) & 0x80)
		{
			m_isSkill_1 = true;
		}


		if (m_pInput_Device->Get_KeyState(DIK_3) & 0x80)
		{
			m_pSound_Device->SoundPlay(23, 0);
			m_isSkill_2 = true;
		}

		//Building Tower
		if (true == m_pKeyCheckerCom->Check_KeyBoardButton_Once(DIK_4))
		{
			if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_SpikyTower", L"Object_SpikyTower")))
				_MSG_BOX("Adding SpikyTower Failed");
			m_isBuildOn = true;
			m_pCursor->Set_isFixity(false);
			m_pCamera->Set_MouseMove(false);
			m_pSound_Device->SoundPlay(27, 0);
		}

		//Start Turn
		if (true == m_pKeyCheckerCom->Check_KeyBoardButton_Once(DIK_H))
		{
			Set_Animation(Ani_WaveStart);
			CTurn*	pTurn = CTurn::Get_Instance();
			if (nullptr == pTurn)
				return;
			pTurn->Start_Wave();
		}

		//PickUp
		if (true == m_pKeyCheckerCom->Check_KeyBoardButton_Once(DIK_E))
			PickUp();
	}

	//Inventory
	if (true == m_pKeyCheckerCom->Check_KeyBoardButton_Once(DIK_I))
	{
		Turn_UI(UI_INVEN);
	}

	//Profile
	if (true == m_pKeyCheckerCom->Check_KeyBoardButton_Once(DIK_P))
	{
		Turn_UI(UI_PROFILE);
	}

	//Esc
	if (true == m_pKeyCheckerCom->Check_KeyBoardButton_Once(DIK_ESCAPE))
	{
		//열려있는창 없애기
		if (nullptr != m_pInventory)
			m_pInventory->Set_SeeAble(false);

		if (nullptr != m_pProfile)
			m_pProfile->Set_SeeAble(false);

		m_pCursor->Set_isFixity(true);
		m_pCamera->Set_MouseMove(true);
	}

	//Create Item
	if (true == m_pKeyCheckerCom->Check_KeyBoardButton_Once(DIK_L))
		Create_Item_ForTest();

}

void CPlayer::Move(const _float & fTimeDelta, const _float& fSpeed)
{
	if (nullptr == m_pMeshCom
		|| nullptr == m_pNaviCom
		|| nullptr == m_pTransformCom)
		return;

	if (m_pInput_Device->Get_KeyState(DIK_W) & 0x80)
		Go_FrontAndBack_OnNavigation(fTimeDelta, fSpeed * -1.f);

	if (m_pInput_Device->Get_KeyState(DIK_S) & 0x80)
		Go_FrontAndBack_OnNavigation(fTimeDelta, fSpeed);

	if (m_pInput_Device->Get_KeyState(DIK_A) & 0x80)
		Go_LeftAndRight_OnNavigation(fTimeDelta, fSpeed);

	if (m_pInput_Device->Get_KeyState(DIK_D) & 0x80)
		Go_LeftAndRight_OnNavigation(fTimeDelta, fSpeed * -1.f);

	//회전
	_uint iCurrentState = m_pMeshCom->Get_CurrentAniIdx();

	if (_long lMouseMoveX = m_pInput_Device->Get_MouseMove(CInput_Device::DIMM_X))
	{
		m_pTransformCom->RotationY(lMouseMoveX * fTimeDelta * 0.05f);
		D3DXMatrixRotationY(&m_matRotY, lMouseMoveX * fTimeDelta * 0.05f);
		if (iCurrentState != Ani_RunForward
			&& false == m_isJump
			&& false == m_isAtt)
			Set_Animation(Ani_TurnLeft);
	}
}

void CPlayer::Go_FrontAndBack_OnNavigation(const _float & fTimeDelta, const _float& fSpeedPerSecond)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pNaviCom
		|| nullptr == m_pColliderCom)
		return;

	_vec3	vNormalVec = _vec3(0.f, 0.f, 0.f);

	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	_vec3	vDir = *m_pTransformCom->Get_Infomation(CTransform::INFO_LOOK);
	D3DXVec3Normalize(&vDir, &vDir);

	_float	fSpeed = fSpeedPerSecond * fTimeDelta;

	_uint iResult = m_pNaviCom->Check_Exist(vPos, vDir, fSpeed, &m_iCurrentIdx, &vNormalVec);

	// 네비 메쉬 안에 있으면
	if (CNavigation::EXIST_IN == iResult || CNavigation::EXIST_NEIGHBOR == iResult)
	{
		//부딪힌 상태가 아닐 때 이동
		if (false == m_pColliderCom->Get_isCollision())
			m_pTransformCom->Go_Straight((fSpeedPerSecond - 1.f) * fTimeDelta);
		//부딪혔으면 슬라이딩 벡터로 이동
		else
			m_pTransformCom->Go_Sliding(*m_pTransformCom->Get_Infomation(CTransform::INFO_LOOK), vNormalVec, fTimeDelta);

		if (false == m_isJump
			&& false == m_isAtt)
			Set_Animation(Ani_RunForward);
	}

	// 네비 메쉬 밖에 있으면
	else if (CNavigation::EXIST_OUT == iResult)
	{
		
		//슬라이딩 벡터로 이동할 위치가 네비 메쉬 안인지 판단하기
		_vec3	vDir = *m_pTransformCom->Get_Infomation(CTransform::INFO_LOOK);
		_vec3	vProj = vNormalVec * D3DXVec3Dot(&vNormalVec, &(vDir * -1.f));
		_vec3	vResult;
		D3DXVec3Normalize(&vResult, &(vDir + vProj));
		iResult = m_pNaviCom->Check_Exist(*m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), vDir + vProj, fTimeDelta * 2.f, &m_iCurrentIdx, &vNormalVec);

		if (CNavigation::EXIST_IN == iResult || CNavigation::EXIST_NEIGHBOR == iResult)
		{
			m_pTransformCom->Go_Sliding(*m_pTransformCom->Get_Infomation(CTransform::INFO_LOOK), vNormalVec, fTimeDelta);
			if (false == m_isJump
				&& false == m_isAtt)
				Set_Animation(Ani_RunForward);
		}
	}
}

void CPlayer::Go_LeftAndRight_OnNavigation(const _float & fTimeDelta, const _float& fSpeedPerSecond)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pNaviCom
		|| nullptr == m_pColliderCom)
		return;

	_vec3	vNormalVec = _vec3(0.f, 0.f, 0.f);

	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	_vec3	vDir = *m_pTransformCom->Get_Infomation(CTransform::INFO_RIGHT);
	D3DXVec3Normalize(&vDir, &vDir);

	_float	fSpeed = fSpeedPerSecond * fTimeDelta;

	_uint iResult = m_pNaviCom->Check_Exist(vPos, vDir, fSpeed, &m_iCurrentIdx, &vNormalVec);

	// 네비 메쉬 안에 있으면
	if (CNavigation::EXIST_IN == iResult || CNavigation::EXIST_NEIGHBOR == iResult)
	{
		//부딪힌 상태가 아닐 때 이동
		if (false == m_pColliderCom->Get_isCollision())
			m_pTransformCom->Go_Direction(vDir, (fSpeedPerSecond - 1.f) * fTimeDelta);
		//부딪혔으면 슬라이딩 벡터로 이동
		else
			m_pTransformCom->Go_Sliding(*m_pTransformCom->Get_Infomation(CTransform::INFO_LOOK), vNormalVec, fTimeDelta);

		if (false == m_isJump
			&& false == m_isAtt)
			Set_Animation(Ani_RunForward);
	}

	// 네비 메쉬 밖에 있으면
	else if (CNavigation::EXIST_OUT == iResult)
	{
		//슬라이딩 벡터로 이동할 위치가 네비 메쉬 안인지 판단하기
		_vec3	vDir = *m_pTransformCom->Get_Infomation(CTransform::INFO_LOOK);
		_vec3	vProj = vNormalVec * D3DXVec3Dot(&vNormalVec, &(vDir * -1.f));
		_vec3	vResult;
		D3DXVec3Normalize(&vResult, &(vDir + vProj));
		iResult = m_pNaviCom->Check_Exist(*m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), vDir + vProj, fTimeDelta * 2.f, &m_iCurrentIdx, &vNormalVec);
		if (CNavigation::EXIST_IN == iResult || CNavigation::EXIST_NEIGHBOR == iResult)
		{
			m_pTransformCom->Go_Sliding(*m_pTransformCom->Get_Infomation(CTransform::INFO_LOOK), vNormalVec, fTimeDelta);
			if (false == m_isJump
				&& false == m_isAtt)
				Set_Animation(Ani_RunForward);
		}
	}
}

void CPlayer::Go_Direction_OnNavigation(const _float & fTimeDelta, const _float & fSpeedPerSecond, const _vec3 & vGoDir)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pNaviCom
		|| nullptr == m_pColliderCom)
		return;

	_vec3	vNormalVec = _vec3(0.f, 0.f, 0.f);

	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	_vec3	vDir = vGoDir;
	D3DXVec3Normalize(&vDir, &vDir);

	_float	fSpeed = fSpeedPerSecond * fTimeDelta;

	_uint iResult = m_pNaviCom->Check_Exist(vPos, vDir, fSpeed, &m_iCurrentIdx, &vNormalVec);

	// 네비 메쉬 안에 있으면
	if (CNavigation::EXIST_IN == iResult || CNavigation::EXIST_NEIGHBOR == iResult)
	{
		//부딪힌 상태가 아닐 때 이동
		if (false == m_pColliderCom->Get_isCollision())
			m_pTransformCom->Go_Direction(vDir, (fSpeedPerSecond - 1.f) * fTimeDelta);
		//부딪혔으면 슬라이딩 벡터로 이동
		else
			m_pTransformCom->Go_Sliding(*m_pTransformCom->Get_Infomation(CTransform::INFO_LOOK), vNormalVec, fTimeDelta);

		if (false == m_isJump
			&& false == m_isAtt)
			Set_Animation(Ani_RunForward);
	}

	// 네비 메쉬 밖에 있으면
	else if (CNavigation::EXIST_OUT == iResult)
	{
		//슬라이딩 벡터로 이동할 위치가 네비 메쉬 안인지 판단하기
		_vec3	vDir = *m_pTransformCom->Get_Infomation(CTransform::INFO_LOOK);
		_vec3	vProj = vNormalVec * D3DXVec3Dot(&vNormalVec, &(vDir * -1.f));
		_vec3	vResult;
		D3DXVec3Normalize(&vResult, &(vDir + vProj));
		iResult = m_pNaviCom->Check_Exist(*m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), vDir + vProj, fTimeDelta * 2.f, &m_iCurrentIdx, &vNormalVec);
		if (CNavigation::EXIST_IN == iResult || CNavigation::EXIST_NEIGHBOR == iResult)
		{
			m_pTransformCom->Go_Sliding(*m_pTransformCom->Get_Infomation(CTransform::INFO_LOOK), vNormalVec, fTimeDelta);
			if (false == m_isJump
				&& false == m_isAtt)
				Set_Animation(Ani_RunForward);
		}
	}
}

void CPlayer::Jump(const _float & fTimeDelta)
{
	if (nullptr == m_pInput_Device
		|| nullptr == m_pTransformCom
		|| nullptr == m_pMeshCom)
		return;
		
	if (m_pInput_Device->Get_KeyState(DIK_SPACE) & 0x80)
	{
		if (m_isJump != true)
		{
			m_fOriginalY = (*m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION)).y;
			m_isJump = true;
			m_isJumpUp = true;
			Set_Animation(Ani_Jump);
		}
	}

	if (true == m_isJump)
	{
		if (true == m_pMeshCom->is_Finished())
			Set_Animation(Ani_Jump_Falling);

		_float fJumpY = (*m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION)).y;
		if (true == m_isJumpUp)
		{
			m_pTransformCom->Jump(JUMP_UP_SPEED * fTimeDelta);
			if (fJumpY - m_fOriginalY > JUMP_AMOUNT)
				m_isJumpUp = false;
		}
		else
		{
			m_pTransformCom->Jump(JUMP_DOWN_SPEED * fTimeDelta * -1.f);
			if (fJumpY - m_fOriginalY < JUMP_END_POINT)
			{
				m_isJump = false;
			}
		}
	}
	//높이계산
	else
	{
		_vec3 vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
		_float fHight = m_pNaviCom->Get_Hight(vPos, &m_iCurrentIdx);
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, vPos.x, fHight, vPos.z);
	}
}

void CPlayer::Basic_Att(const _float & fTimeDelta)
{
	if (nullptr == m_pKeyCheckerCom
		|| nullptr == m_pMeshCom)
		return;

	if (true == m_pKeyCheckerCom->Check_MouseButton_Once(CKeyChecker::DIM_LBUTTON))
	{
		m_isAtt = true;
		++m_iBasicAttCnt;
	}

	if (true == m_isAtt)
	{
		switch (m_iBasicAttCnt)
		{
		case 1:
			//3콤보 후 이어서 다시 공격
			if (Ani_MeleeAttack3_medium == m_pMeshCom->Get_CurrentAniIdx())
			{
				if (true == m_pMeshCom->is_Finished())
				{
					m_iBasicAttCnt = 1;
					Set_Animation(Ani_MeleeAttack1_medium);

					if (nullptr != m_pWeapon)
						m_pWeapon->Set_isAtt(true);
				}
			}
			// Att 1
			else
			{
				if (Ani_MeleeAttack1_medium != m_pMeshCom->Get_CurrentAniIdx())
				{

					Set_Animation(Ani_MeleeAttack1_medium);
					if (nullptr != m_pWeapon)
						m_pWeapon->Set_isAtt(true);

					m_pSound_Device->SoundPlay(24, 0);
				}

				m_iBasicAttCnt = 1;
			}

			// Att 1 Finish
			if (Ani_MeleeAttack1_medium == m_pMeshCom->Get_CurrentAniIdx() && true == m_pMeshCom->is_Finished())
			{
				m_iBasicAttCnt = 0;
				m_isAtt = false;
			}
			break;

		case 2:
			//잘못 들어 올 경우
			if (Ani_MeleeAttack3_medium == m_pMeshCom->Get_CurrentAniIdx())
				m_iBasicAttCnt = 1;

			// Att 2
			if (Ani_MeleeAttack1_medium == m_pMeshCom->Get_CurrentAniIdx() && true == m_pMeshCom->is_Finished())
			{
				m_iBasicAttCnt = 2;
				Set_Animation(Ani_MeleeAttack2_medium);

				if (nullptr != m_pWeapon)
					m_pWeapon->Set_isAtt(true);

				m_pSound_Device->SoundPlay(24, 0);
			}

			//Att 2 Finish
			if (Ani_MeleeAttack2_medium == m_pMeshCom->Get_CurrentAniIdx() && true == m_pMeshCom->is_Finished())
			{
				m_iBasicAttCnt = 0;
				m_isAtt = false;
			}
			break;

		case 3:
			//잘못 들어 올 경구
			if (Ani_MeleeAttack1_medium == m_pMeshCom->Get_CurrentAniIdx())
				m_iBasicAttCnt = 2;

			// Att 3
			if (Ani_MeleeAttack2_medium == m_pMeshCom->Get_CurrentAniIdx() && true == m_pMeshCom->is_Finished())
			{
				m_iBasicAttCnt = 3;
				Set_Animation(Ani_MeleeAttack3_medium);

				if (nullptr != m_pWeapon)
					m_pWeapon->Set_isAtt(true);

				m_pSound_Device->SoundPlay(25, 0);
			}

			// Att 3 Finish
			if (Ani_MeleeAttack3_medium == m_pMeshCom->Get_CurrentAniIdx() && true == m_pMeshCom->is_Finished())
			{
				m_iBasicAttCnt = 0;
				m_isAtt = false;
			}
			break;

		default:
			m_iBasicAttCnt = 0;
			m_isAtt = false;
			break;
		}
	}
}

void CPlayer::PullOut(const _float & fTimeDelta)
{
	if (nullptr == m_pTransformCom)
		return;

	//Compute pulled direction
	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	m_vPulledPos.y = vPos.y;

	_vec3	vDir = m_vPulledPos - vPos;

	//If ther player reach to the goal
	if (PULL_END_POINT > D3DXVec3Length(&vDir))
		return;

	Go_Direction_OnNavigation(fTimeDelta, m_fPulledSpeed, vDir);
}

void CPlayer::Create_Item_ForTest(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pObject_Manager)
		return;

	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	ITEMINFO	tInfo;
	ZeroMemory(&tInfo, sizeof(ITEMINFO));

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_ItemIcon", L"Object_ItemIcon", &_vec3(vPos.x, vPos.y + 1.f, vPos.z))))
		return;
	CItemIcon*	pItem = dynamic_cast<CItemIcon*>(m_pObject_Manager->Get_LastGameObject(SCENE_STAGE, L"Layer_ItemIcon"));
	if (nullptr == pItem)
		return;
	tInfo.pItemName = L"Leather_Chest";
	tInfo.iItemKind = CInventory::ITEM_DEF_CHEST;
	tInfo.fItemStats = 5.f;
	pItem->Set_ItemInfo(L"Com_Mesh_Leather_Chest_Pickup", &tInfo);

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_ItemIcon", L"Object_ItemIcon", &_vec3(vPos.x - 1.f, vPos.y + 1.f, vPos.z))))
		return;
	pItem = dynamic_cast<CItemIcon*>(m_pObject_Manager->Get_LastGameObject(SCENE_STAGE, L"Layer_ItemIcon"));
	if (nullptr == pItem)
		return;
	tInfo.pItemName = L"Leather_Boots";
	tInfo.iItemKind = CInventory::ITEM_DEF_BOOTS;
	tInfo.fItemStats = 5.f;
	pItem->Set_ItemInfo(L"Com_Mesh_Leather_Boots_Pickup", &tInfo);

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_ItemIcon", L"Object_ItemIcon", &_vec3(vPos.x - 2.f, vPos.y + 1.f, vPos.z))))
		return;
	pItem = dynamic_cast<CItemIcon*>(m_pObject_Manager->Get_LastGameObject(SCENE_STAGE, L"Layer_ItemIcon"));
	if (nullptr == pItem)
		return;
	tInfo.pItemName = L"Chain_Chest";
	tInfo.iItemKind = CInventory::ITEM_DEF_CHEST;
	tInfo.fItemStats = 5.f;
	pItem->Set_ItemInfo(L"Com_Mesh_Chain_Chest_Pickup", &tInfo);

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_ItemIcon", L"Object_ItemIcon", &_vec3(vPos.x - 3.f, vPos.y + 1.f, vPos.z))))
		return;
	pItem = dynamic_cast<CItemIcon*>(m_pObject_Manager->Get_LastGameObject(SCENE_STAGE, L"Layer_ItemIcon"));
	if (nullptr == pItem)
		return;
	tInfo.pItemName = L"Chain_Boots";
	tInfo.iItemKind = CInventory::ITEM_DEF_BOOTS;
	tInfo.fItemStats = 5.f;
	pItem->Set_ItemInfo(L"Com_Mesh_Chain_Boots_Pickup", &tInfo);
}

void CPlayer::PickUp(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pInventory
		|| nullptr == m_pObject_Manager
		|| nullptr == m_pSound_Device)
		return;

	_float	fDistance = 0.f;
	_uint	iItemIdx = 0;
	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

	//가까이 있는 아이템 탐색
	_uint	iItemNum = m_pObject_Manager->Get_GameObjectNum(SCENE_STAGE, L"Layer_ItemIcon");
	for (_uint i = 0; i < iItemNum; ++i)
	{
		CItemIcon*	pItem = dynamic_cast<CItemIcon*>((m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_ItemIcon", i)));
		if (nullptr == pItem)
			return;
		CTransform*	pItemTransform = dynamic_cast<CTransform*>((pItem->Get_Component(L"Com_Transform")));
		if (nullptr == pItemTransform)
			return;

		_vec3	vItemPos = *pItemTransform->Get_Infomation(CTransform::INFO_POSITION);

		if (0 == i)
			fDistance = D3DXVec3Length(&(vItemPos - vPos));
		else
		{
			_float	fDistance_Dest = D3DXVec3Length(&(vItemPos - vPos));
			if (fDistance > fDistance_Dest)
			{
				fDistance = fDistance_Dest;
				iItemIdx = i;
			}
		}
	}

	//가까우면 집는 모션
	if (fDistance > ITEM_END_DISTANCE)
		return;
	else
		Set_Animation(Ani_PickupItem);

	//해당 아이템 정보 받아오기
	CItemIcon*	pItem = dynamic_cast<CItemIcon*>((m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_ItemIcon", iItemIdx)));
	if (nullptr == pItem)
		return;
	ITEMINFO*	pInfo = pItem->Get_ItemInfo();

	//장비함에 추가
	m_pInventory->Add_Item(pInfo);
	m_pSound_Device->SoundPlay(18, 0);

	//해당 아이템 아이콘 없애기
	pItem->Set_isDead(true);
}

void CPlayer::Turn_UI(const UIKIND & eUIKind)
{
	CUI*	pUI = nullptr;
	switch(eUIKind)
	{
	case (UI_INVEN) :
	{		
		if (nullptr == m_pInventory)
			return;
		pUI = m_pInventory;
		break;
	}
	case (UI_PROFILE):
	{
		if (nullptr == m_pProfile)
			return;
		pUI = m_pProfile;
		break;
	}
	}

	_bool	isSeeAble = pUI->Get_SeeAble();

	pUI->Set_SeeAble(!isSeeAble);
	m_pCursor->Set_isFixity(isSeeAble);
	m_pCamera->Set_MouseMove(isSeeAble);

	m_pSound_Device->SoundPlay(17, 0);
}

void CPlayer::Update_Collider(void)
{
	if (nullptr == m_pColliderCom)
		return;
	m_pColliderCom->Update_Collider();
}

void CPlayer::Add_Hp(const _float & fHpAmount)
{
	if (false == m_isDeadAni)
	{
		//Hurt
		if (fHpAmount < 0)
		{
			Set_Animation(Ani_Hurt);

			_float	fDamege = fHpAmount + m_tUnitInfo.fDef;
			m_tUnitInfo.fHp += fDamege;

			// Blood Effect
			if (nullptr != m_pTransformCom
				&& nullptr != m_pObject_Manager)
				m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Blood", L"Object_Blood", m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION));

			// Damage Font
			if (nullptr != m_pObject_Manager
				&& nullptr != m_pTransformCom)
			{
				_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
				if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Font_Damage", L"Object_Font_Damage", &_vec3(vPos.x, vPos.y + DAMEGE_HIGHT, vPos.z))))
					return;
				CFont_Damage* pFont = dynamic_cast<CFont_Damage*>(m_pObject_Manager->Get_LastGameObject(SCENE_STAGE, L"Layer_Font_Damage"));
				if (nullptr != pFont)
					pFont->Set_Font((_uint)(fDamege * -1.f));
			}

			if (nullptr != m_pSound_Device)
				m_pSound_Device->SoundPlay(20, 0);
		}
		//Heal
		else
			m_tUnitInfo.fHp += fHpAmount;

		//Over
		if (m_tUnitInfo.fMaxHp < m_tUnitInfo.fHp)
			m_tUnitInfo.fHp = m_tUnitInfo.fMaxHp;

		//Die
		if (0.f >= m_tUnitInfo.fHp)
		{
			if(nullptr != m_pColliderCom)
				m_pColliderCom->Set_isTargetDead(true);
			m_isDeadAni = true;
		}
	}
}

_int CPlayer::Update_GameObject(const _float & fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	Update_Collider();
	InputProcess(fTimeDelta);

	if(true == m_isPulledBack)
		PullOut(fTimeDelta);

	if (true == m_isSkill_0)
	{
		Skill_0(fTimeDelta);
	}

	else if (true == m_isSkill_1)
	{
		Skill_1(fTimeDelta);
	}

	else if (true == m_isSkill_2)
	{
		Skill_2(fTimeDelta);
	}

	if (true == m_isDeadAni)
	{
		Set_Animation(Ani_Death);
		if (nullptr != m_pMeshCom)
		{
			if (true == m_pMeshCom->is_Finished())
				m_isDead = true;
		}
	}

	return _RUN;
}

_int CPlayer::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UNIT, this);

	return _int();
}

void CPlayer::Render_GameObject(void)
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
		_MSG_BOX("CPlayer SetUp_ConstantTable Failed");
		return;
	}

	pEffect->Begin(nullptr, 0);

	if (Ani_Hurt == m_pMeshCom->Get_CurrentAniIdx())
		m_pMeshCom->Render_Mesh(pEffect, 1);
	else
		m_pMeshCom->Render_Mesh(pEffect, 0);

	pEffect->End();	

	if (nullptr == m_pColliderCom)
		return;

	m_pColliderCom->Render_Collider();
}

void CPlayer::Skill_0(const _float & fTimeDelta)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pObject_Manager)
		return;

	m_fAccTimeDeltaForSkill_0 += fTimeDelta;

	_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

	//SKILL0_PER_TIME 마다 1번씩 힐
	if (m_fAccTimeDeltaForSkill_0 > SKILL0_PER_TIME)
	{
		m_tUnitInfo.fHp += SKILL0_PER_HEAL_AMOUNT;

		// Effect
		for (_uint i = 0; i < SKILL0_EFFECT_MAXCNT; ++i)
		{
			_uint	iRand = rand() % 2;
			_float	fRandX = 0.f, fRandY = 0.f, fRandZ = 0.f;
			fRandY = _float(rand() % 100) * 0.01f;

			if (0 == iRand)
				fRandX = _float((rand() % SKILL0_EFFECT_RAND_VALUE) * 0.1);
			else
				fRandX = _float((rand() % SKILL0_EFFECT_RAND_VALUE) * -0.1);

			_vec3	vRight = *m_pTransformCom->Get_Infomation(CTransform::INFO_RIGHT);
			vRight *= fRandX;
			_vec3	vParticlePos = vPos + vRight;
			vParticlePos.y += fRandY;
			if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_HealParticle", L"Object_HealParticle", &vParticlePos)))
				return;
		}

		// 한 번 끝
		m_fAccTimeDeltaForSkill_0 = 0.f;
		++m_iSkill_0_Cnt;
	}

	//종료
	if (SKILL0_MAXCNT == m_iSkill_0_Cnt)
	{
		m_iSkill_0_Cnt = 0;
		m_isSkill_0 = false;
	}
}

void CPlayer::Skill_1(const _float & fTimeDelta)
{
	if (false == m_isSkill_1_Init)
	{
		Set_Animation(Ani_Circluarslice);
		m_fAccTimeDeltaForSkill_1 += fTimeDelta;
		if (m_fAccTimeDeltaForSkill_1 > SKILL1_READY_TIME)
		{
			m_pSound_Device->SoundPlay(22, 0);
			m_isSkill_1_Init = true;
		}
	}

	if (nullptr != m_pWeapon)
	{
		m_pWeapon->Set_isAtt(true);
		m_pWeapon->Set_isKnockBack(true);
	}


	if (true == m_pMeshCom->is_Finished())
	{
		Set_Animation(Ani_Idle);
		m_pWeapon->Set_isAtt(false);
		m_isSkill_1 = false;
		m_isSkill_1_Init = false;
	}
}

void CPlayer::Skill_2(const _float & fTimeDelta)
{
	m_fAccTimeDeltaForSkill_2 += fTimeDelta;

	if (m_fAccTimeDeltaForSkill_2 > SKILL2_READY_TIME)
	{
		if (false == m_isSkill_2_Init)
		{
			m_tUnitInfo.fAtt *= SKILL2_BUFF_MULTI_VALUE;
			m_tUnitInfo.fDef *= SKILL2_BUFF_MULTI_VALUE;

			_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
			if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_PowerUpAura", L"Object_PowerUpAura", &vPos)))
				return;

			CPowerUpAura*	pEffect = dynamic_cast<CPowerUpAura*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_PowerUpAura"));
			if (nullptr != pEffect)
				pEffect->Set_Target(m_pTransformCom);

			m_isSkill_2_Init = true;
		}
	}

	if (m_fAccTimeDeltaForSkill_2 > SKILL2_READY_TIME + SKILL2_CONTINUE_TIME)
	{
		CGameObject*	pEffect = m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_PowerUpAura");
		if(nullptr != pEffect)
			pEffect->Set_isDead(true);

		m_tUnitInfo.fAtt /= SKILL2_BUFF_MULTI_VALUE;
		m_tUnitInfo.fDef /= SKILL2_BUFF_MULTI_VALUE;
		m_isSkill_2 = false;
		m_fAccTimeDeltaForSkill_2 = 0.f;
	}
}

CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer* pInstance = new CPlayer(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CPlayer Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CPlayer Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CPlayer Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free(void)
{
	if (false == m_isClone)
	{
		for (auto& iter : m_vecAniInfo)
			Safe_Delete(iter);
		m_vecAniInfo.clear();
	}

	if (nullptr != m_pWeapon)
		Safe_Release(m_pWeapon);

	Safe_Release(m_pInventory);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNaviCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pKeyCheckerCom);

	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pSound_Device);

	CUnit::Free();
}
