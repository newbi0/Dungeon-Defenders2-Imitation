#pragma once
#include "Unit.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CNavigation;
	class CMesh_Dynamic;
	class CShader;
	class CCollider;
	class CAStar;
	class CObject_Manager;
	class CSound_Device;
}

class CHpBar_Boss;
class CHpBarFR_Boss;
class CHpBarBG_Boss;
class CDemonLord final : public CUnit
{
public:
	enum eANINAME {
		Ani_TurnLeft, Ani_Shock_Ground, Ani_Shock_Air, Ani_MoveForward, Ani_Idle,
		Ani_Hurt, Ani_Fly_Right, Ani_Fly_Pound_Loop, Ani_Fly_Pound_End, Ani_Fly_Left,
		Ani_Fly_Launch, Ani_Fly_Hover, Ani_Fly_Forward, Ani_Fly_Fireball, Ani_Fly_Backward,
		Ani_Death, Ani_Cinematic_Landing, Ani_Cinematic_Cutthroat, Ani_Attack_SummonFlame, Ani_Attack_Slice,
		Ani_Attack_Right, Ani_Attack_Pound, Ani_Attack_Left, Ani_END	};
private:
	explicit CDemonLord(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CDemonLord(const CDemonLord& rhs);
	virtual ~CDemonLord(void) = default;
public:
	virtual const _matrix* Get_WorldMatrix(void) const;
	const _matrix* Get_matRotY(void) const;
public:
	void Set_Target(CUnit* pTarget);
public:
	virtual void Add_Hp(const _float& fHpAmount);
private:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(void);
	HRESULT Ready_AniInfo(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	void Set_Animation(const eANINAME& eAniName);
	void Set_Direction(const _vec3& vDir, const _bool& isFront = false, const _float& fDistance = 0.3f);
private:
	void Update_Collider(void);
	void Collider_Att(void);
private:
	void Att_Basic(const _float & fTimeDelta);
	void Skill_0(const _float & fTimeDelta);
	void Skill_1(const _float & fTimeDelta);
	void Skill_2(const _float & fTimeDelta);
	void Skill_3(const _float & fTimeDelta);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
private:
	_uint					m_iCurrentIdx = 0;
	_float					m_fTimeDelta = 0.f;
	_vec3					m_vMin, m_vMax;
	_matrix					m_matRotY;
	CUnit*					m_pTargetUnit = nullptr;
//Component
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CNavigation*			m_pNaviCom = nullptr;
	CMesh_Dynamic*			m_pMeshCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CCollider*				m_pLeftColliderCom = nullptr;
	CCollider*				m_pRightColliderCom = nullptr;
	CAStar*					m_pAStarCom = nullptr;
//SingleTone
	CObject_Manager*		m_pObject_Manager = nullptr;
	CSound_Device*			m_pSound_Device = nullptr;
//HpBar
private:
	CHpBar_Boss*			m_pHpBar = nullptr;
	CHpBarFR_Boss*			m_pHpBarFR = nullptr;
	CHpBarBG_Boss*			m_pHpBarBG = nullptr;
//For. AStar
private:
	_vec3					m_vTargetPos;
	_vec3					m_vCrystalPos;
	list<_vec3>				m_listBest;
//For.Jump
private:
	_bool					m_isJump = false;
	_bool					m_isJumpUp = false;
	_float					m_fOriginalY = 0.f;
private:
	_bool					m_isShock = false;
	_bool					m_isAttOnce = false;
	_bool					m_isFollower = false;
	_bool					m_isAniPlay = true;
	_bool					m_isEnter = true;
	_bool					m_isAtt = false;
	_uint					m_iBasicAtt = 0;
//For.Skill
private:
	_bool					m_isInitSkill0 = false;
	_bool					m_isSkill0 = false;
	_bool					m_isInitSkill1 = false;
	_bool					m_isSkill1 = false;
	_bool					m_isInitSkill2 = false;
	_bool					m_isSkill2 = false;
	_bool					m_isInitSkill3 = false;
	_bool					m_isSkill3 = false;
	_bool					m_isGage = false;
	_float					m_fTimeAcc = 0.f;			//For Skill Bar
	_float					m_fAccTimeDelta_ForSkill3 = 0.f;
	_float					m_fAccTimeDelta2_ForSkill3 = 0.f;
private:
	vector<ANIINFO*>			m_vecAniInfo;
	typedef vector<ANIINFO*>	VECANIINFO;
public:
	static CDemonLord* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

