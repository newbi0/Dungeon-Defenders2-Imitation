#pragma once
#include "Unit.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CMesh_Static;
	class CShader;
	class CCollider;
	class CNavigation;
	class CKeyChecker;
	class CObject_Manager;
	class CSound_Device;
}

class CHpBarBG_Enemy;
class CHpBar_Enemy;
class CUnit;
class CSpikyTower final : public CUnit
{
private:
	explicit CSpikyTower(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSpikyTower(const CSpikyTower& rhs);
	virtual ~CSpikyTower(void) = default;
public:
	_bool Check_CrushToBackOfTower(void);
	virtual void Add_Hp(const _float& fHpAmount);
private:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
private:
	void Update_Collider(void);
	void Collision_Att(void);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CMesh_Static*			m_pMeshCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CCollider*				m_pAttColliderCom = nullptr;
	CCollider*				m_pBackColliderCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CKeyChecker*			m_pKeyCheckerCom = nullptr;
	CObject_Manager*		m_pObject_Manager = nullptr;
	CSound_Device*			m_pSound_Device = nullptr;
private:
	CUnit*					m_pTargetUnit = nullptr;
	CHpBarBG_Enemy*				m_pHpBarBG = nullptr;
	CHpBar_Enemy*				m_pHpBar = nullptr;
private:
	_vec3					m_vMin, m_vMax;
private:
	_bool					m_isBuild = false;
	_bool					m_isPos = false;
	_bool					m_isAtt = false;
	_float					m_fTimeDelta = 0.f;
public:
	static CSpikyTower* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

