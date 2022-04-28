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
	class CFrustum_Culling;
	class CObject_Manager;
	class CSound_Device;
}

class CHpBar_Enemy;
class CHpBarBG_Enemy;
class CGoblin final : public CUnit
{
public:
	enum eANINAME {
		Ani_TurnLeft, Ani_Shok, Ani_MoveForward, Ani_Jump, Ani_Idle,
		Ani_Hurt, Ani_Death, Ani_Cough, Ani_Attack2, Ani_Attack1, Ani_END	};
private:
	explicit CGoblin(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CGoblin(const CGoblin& rhs);
	virtual ~CGoblin(void) = default;
public:
	const _matrix* Get_WorldMatrix(void) const;
	const _matrix* Get_matRotY(void) const;
public:
	void Set_Target(CUnit* pTarget);
public:
	virtual void Add_Hp(const _float& fHpAmount);
	virtual void Set_Knock_Back(void);
	virtual void Knock_Back(const _float& fTimeDelta);
private:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(void);
	HRESULT Ready_AniInfo(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	void Set_Animation(const eANINAME& eAniName);
	void Set_Direction(const _vec3& vDir);
private:
	void Update_Collider(void);
	void Collider_Att(const _float& fTimeDelta);
	void Compute_TargetPos(void);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
private:
	_uint					m_iCurrentIdx = 0;
	_uint					m_iTest = 0;
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CNavigation*			m_pNaviCom = nullptr;
	CMesh_Dynamic*			m_pMeshCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CCollider*				m_pClubColliderCom = nullptr;
	CAStar*					m_pAStarCom = nullptr;
	CFrustum_Culling*		m_pCullingCom = nullptr;
	CObject_Manager*		m_pObject_Manager = nullptr;
	CSound_Device*			m_pSound_Device = nullptr;
private:
	CHpBar_Enemy*			m_pHpBar = nullptr;
	CHpBarBG_Enemy*			m_pHpBarBG = nullptr;
	CUnit*					m_pTargetUnit = nullptr;
	_bool					m_isFollower = false;
private:
	_vec3					m_vTargetPos;
	_vec3					m_vCrystalPos;
	vector<STAIRINFO*>		m_vecStairInfo;
	list<_vec3>				m_listBest;
	_float					m_fTimeDelta = 0.f;
	_float					m_fAccAtt = 0.f;
	_matrix					m_matRotY;
private:
	vector<ANIINFO*>			m_vecAniInfo;
	typedef vector<ANIINFO*>	VECANIINFO;
//For.Jump
private:
	_bool					m_isJump = false;
	_bool					m_isJumpUp = false;
	_float					m_fOriginalY = 0.f;
	_bool					m_isPlayAni = true;
private:
	_uint					m_iBasicAtt = 0;
	_bool					m_isAtt = false;
	_bool					m_isAttOnce = false;
	_bool					m_isAttMotion = false;
public:
	static CGoblin* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

