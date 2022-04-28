#pragma once
#include "GameObject.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CMesh_Static;
	class CShader;
	class CCollider;
	class CObject_Manager;
}

class CUnit;
class CWeapon abstract : public CGameObject
{
protected:
	explicit CWeapon(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CWeapon(const CWeapon& rhs);
	virtual ~CWeapon(void) = default;
public:
	virtual const _matrix* Get_WorldMatrix(void) const;
public:
	virtual void Set_isAtt(const _bool& isAtt) {
		m_isAtt = isAtt;	}
	virtual void Set_isKnockBack(const _bool& isKnockBack) {
		m_isKnockBack = isKnockBack;	}
protected:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	virtual HRESULT Ready_Component(void);
	virtual	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
protected:
	virtual void Update_Collider(void);
protected:
	virtual void Collision_Att(const _tchar* pTargetTag)_PURE;
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
protected: 
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CMesh_Static*			m_pMeshCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CObject_Manager*		m_pObject_Manager = nullptr;
protected:
	CTransform*				m_pRelevent_TransformCom = nullptr;
	_matrix*				m_pHandMatrix = nullptr;
	_matrix					m_matParent;
	_matrix					m_ColliderMatrix;
protected:
	CUnit*					m_pReleventUnit = nullptr;
	CUnit*					m_pTargetUnit = nullptr;
protected:
	_vec3					m_vMin, m_vMax;
	_float					m_fTimeDelta = 0.f;
	_float					m_fAccAtt = 0.f;
protected:
	_bool					m_isAtt = false;
	_bool					m_isKnockBack = false;
public:
	virtual CGameObject* Clone(const _vec3* pPos = nullptr)_PURE;
	virtual CGameObject* Clone(const _matrix& matWorld)_PURE;
protected:
	virtual void Free(void);
};

