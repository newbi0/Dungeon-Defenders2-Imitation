#pragma once
#include "Unit.h"

namespace Engine
{
	//Component
	class CRenderer;
	class CTransform;
	class CNavigation;
	class CMesh_Dynamic;
	class CShader;
	class CCollider;
	class CKeyChecker;

	//SingleTone
	class CObject_Manager;
	class CSound_Device;
}

class CInventory;
class CEquipmentBox;
class CWeapon;
class CCursor;
class CCamera_Follow;
class CPlayer final : public CUnit
{
public:
	enum UIKIND { UI_INVEN, UI_PROFILE, UI_END };
	enum ANINAME	{
		Ani_WaveWave, Ani_Win, Ani_WaveStart, Ani_Upgrade, Ani_TurnLeft,
		Ani_SummonStop, Ani_Summon_Place, Ani_Summon, Ani_Spawn, Ani_RunForward,
		Ani_Repair, Ani_PickupItem, Ani_Move_Right, Ani_Move_Left, Ani_Move_BackWard,
		Ani_MeleeAttack3_medium, Ani_MeleeAttack2_medium, Ani_MeleeAttack1_medium, Ani_Lose, Ani_LevelUp,
		Ani_Jump_Falling, Ani_Jump, Ani_Idle_LowHealth, Ani_Idle, Ani_Hurt,
		Ani_Heal, Ani_Death, Ani_Detonate, Ani_Circluarslice, Ani_CallOut,
		Ani_Blocking_Large_Finch, Ani_Blocking_Large, Ani_ActivateBloodRage, Ani_END	};
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer(void) = default;
public:
	virtual const _matrix* Get_WorldMatrix(void) const;
	virtual const _matrix* Get_matRotY(void) const;
public:
	void Set_Weapon(CWeapon* pWeapon);
	void Set_Animation(const ANINAME& eAniName);
	void Set_isBuild(_bool isBuildOn);
	void Set_PullOut(const _bool& isPullBack, const _vec3& vPullBack, const _float& fPulledSpeed);
public:
	virtual void Add_Hp(const _float& fHpAmount);
private:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(void);
	HRESULT Ready_AniInfo(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
private:
	void InputProcess(const _float& fTimeDelta);
	void Move(const _float& fTimeDelta, const _float& fSpeed);
	void Go_FrontAndBack_OnNavigation(const _float& fTimeDelta, const _float& fSpeedPerSecond);
	void Go_LeftAndRight_OnNavigation(const _float& fTimeDelta, const _float& fSpeedPerSecond);
	void Go_Direction_OnNavigation(const _float& fTimeDelta, const _float& fSpeedPerSecond, const _vec3& vDir);
	void Jump(const _float& fTimeDelta);
	void Basic_Att(const _float& fTimeDelta);
	void PullOut(const _float& fTimeDelta);
	void Create_Item_ForTest(void);
	void PickUp(void);
	void Turn_UI(const UIKIND& eUIKind);
	void Update_Collider(void);
private:
	void Skill_0(const _float& fTimeDelta);
	void Skill_1(const _float& fTimeDelta);
	void Skill_2(const _float& fTimeDelta);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
private:
	_uint					m_iCurrentIdx = 0;
	_bool					m_isPlayAni = true;
	_bool					m_isBuildOn = false;		//is Tower Builded
private:
	//Component
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CNavigation*			m_pNaviCom = nullptr;
	CMesh_Dynamic*			m_pMeshCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CKeyChecker*			m_pKeyCheckerCom = nullptr;
	//SingleTone
	CObject_Manager*		m_pObject_Manager = nullptr;
	CSound_Device*			m_pSound_Device = nullptr;
//UI
private:
	CInventory*				m_pInventory = nullptr;
	CEquipmentBox*				m_pProfile = nullptr;
private:
	//Camera
	CCamera_Follow*			m_pCamera = nullptr;
private:
	CWeapon*				m_pWeapon = nullptr;
	CCursor*				m_pCursor = nullptr;
private:
	_float					m_fTimeDelta = 0.f;
	_matrix					m_matRotY;
private:
	vector<ANIINFO*>			m_vecAniInfo;
	typedef vector<ANIINFO*>	VECANIINFO;
//For.Jump
private:
	_bool					m_isJump = false;
	_bool					m_isJumpUp = false;
	_float					m_fOriginalY = 0.f;
//For.Skill and Att
private:
	_uint					m_iBasicAttCnt = 0;
	_bool					m_isAtt = false;
	_bool					m_isSkill_0 = false;
	_bool					m_isSkill_1 = false;
	_bool					m_isSkill_1_Init = false;
	_bool					m_isSkill_2 = false;
	_bool					m_isSkill_2_Init = false;
	_uint					m_iSkill_0_Cnt = 0;
	_float					m_fAccTimeDeltaForSkill_0 = 0.f;
	_float					m_fAccTimeDeltaForSkill_1 = 0.f;
	_float					m_fAccTimeDeltaForSkill_2 = 0.f;
//For.Pull Back
private:
	_bool					m_isPulledBack = false;
	_vec3					m_vPulledPos;
	_float					m_fPulledSpeed = 0.f;
public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};


