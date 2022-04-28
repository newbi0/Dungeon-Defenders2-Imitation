#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "GameObject.h"

_BEGIN(Engine)

class CCollider;
class CComponet;
class CLayer final : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer() = default;
public:
	CGameObject* Get_GameObject(const _uint& iIndex);
	CGameObject* Get_GameObject_ApplyToPosition(const _vec3* pPos);
	CGameObject* Get_LastGameObject(void);
	const CComponent* Get_Component(const _tchar* pComponentTag, const _uint& iIndex);
	const _uint Get_GameObjectNum(void) {
		return m_ObjectList.size();	}
	const _bool Get_isHitZone(void);
public:
	void Set_HitZone(const CGameObject::HITZONETYPE & eType, const _uint& iSCENEID);
public:
	HRESULT Ready_Layer(void);
	HRESULT Add_GameObject(CGameObject* pGameObject);
	_int Update_Layer(const _float& fTimeDelta);
	_int LastUpdate_Layer(const _float& fTimeDelta);
public:
	_bool Check_Collision_AABB(CCollider* pCollider, CGameObject** ppTarget);
	_bool Check_Collision_OBB(CCollider * pCollide, CGameObject** ppTarget, const _tchar* pColliderName);
public:
	void Clear_DeadObject(void);
	void Clear_GameObject(void);
private:
	list<CGameObject*>				m_ObjectList;
	typedef list<CGameObject*>		OBJECTLIST;
public:
	static CLayer* Create(void);
	virtual void Free(void);
};

_END