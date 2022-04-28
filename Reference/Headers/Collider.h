#pragma once
#include "Component.h"

#include "Transform.h"

_BEGIN(Engine)

class CShader;
class CInput_Device;
class _ENGINE_DLL CCollider final :
	public CComponent
{
private:
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CCollider(void) = default;
public:
	enum TYPE { TYPE_AABB, TYPE_OBB };
	enum KIND { KIND_COLLEAGUE, KIND_ENEMY, KIND_OBJECT, KIND_END };
public:
	const _bool Get_isDead(void) const {
		return m_isDead;	}
	const _bool Get_isTargetDead(void) const {
		return m_isTargetDead;	}
	const _vec3* Get_Min(void) const {
		return &m_vMin;}
	const _vec3* Get_Max(void) const {
		return &m_vMax;}
	const _bool Get_isCollision(void) const {
		return m_isCollision;	}
	_vec3 Get_Center(void) const {
		return m_vCenter;	}
	_vec3 Get_ProjAxis(const _uint& iIndex) const {
		return m_vProjAxis[iIndex];	}
	_vec3 Get_AlignAxis(const _uint& iIndex) const {
		return m_vAlignAxis[iIndex];	}
	const _matrix* Get_RelevantWorldMatrix(void) {
		_matrix	matIden;
		D3DXMatrixIdentity(&matIden);
		if (matIden != m_matWorld)
			return &m_matWorld;
		return m_pRelevant_TransformCom->Get_WorldMatrix();	}
	const _matrix* Get_RelevantWorldMatrix_NotRot(void) {
		return m_pRelevant_TransformCom->Get_WorldMatrix_NotRot();	}
public:
	void Set_isDead(const _bool& isDead) {
		m_isDead = isDead;	}
	void Set_isTargetDead(const _bool& isTargetDead) {
		m_isTargetDead = isTargetDead;	}
	HRESULT Set_Collider(TYPE eType, const _vec3 * pMin, const _vec3 * pMax, CTransform* pTransform);
	void Set_isCollision(const _bool& isCollision) {
		m_isCollision = isCollision;	}
	void Set_CollisionWorld(const _matrix* matWorld) {
		m_matWorld = *matWorld;	}
	void Add_listCollision(const CCollider* pTargetCollider) {
		m_listCollision.push_back(pTargetCollider);	}
	void Delete_listCollision(const CCollider* pCollider);
public:
	HRESULT Ready_Collider(CShader* pShader);
	void Update_Collider(void);
	void Render_Collider(void);
public:
	void RotationX_Collider(const _float& fAngle);
	void RotationY_Collider(const _float& fAngle);
	void RotationZ_Collider(const _float& fAngle);
public:
	_bool Collision_AABB(CCollider* pTargetCollider);
	_bool Collision_OBB(CCollider** ppTargetCollider, _bool isAddTargetList = true);
private:
	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;
private:
	_vec3			m_vCenter;
	_vec3			m_vProjAxis[3];
	_vec3			m_vAlignAxis[3];
	TYPE			m_eType;
private:
	_matrix			m_matWorld;	//Collider의 world Matrix
	_vec3			m_vMin, m_vMax;
	_bool			m_isCollision = false;
	_uint			m_iIndex = 0;
	_bool			m_isTargetDead = false;	//해당 객체가 죽어있는지
	_bool			m_isDead = false;
private:
	CShader*		m_pShaderCom = nullptr;
	CTransform*		m_pRelevant_TransformCom = nullptr;
	CInput_Device*	m_pInput_Device = nullptr;
private:
	list<const CCollider*>			m_listCollision;	//현재 부딪치고있는 Collider
	typedef	list<const CCollider*>	LISTCOLLISION;
public:
	static CCollider* Create(LPDIRECT3DDEVICE9 pGraphic_Device, CShader* pShader);
	virtual CComponent* Clone(void);
private:
	virtual void Free(void);
};

_END