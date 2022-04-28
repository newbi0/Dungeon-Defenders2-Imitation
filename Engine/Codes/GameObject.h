#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "Component_Manager.h"
#include "Input_Device.h"

_BEGIN(Engine)

class CTexture;
class CComponent;
class _ENGINE_DLL CGameObject abstract : public CBase
{
public:
	enum HITZONETYPE { TYPE_CIRCLE, TYPE_DONUT, TYPE_END };
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;
public:
	CComponent* Get_Component(const _tchar* pComponentTag);
	_float Get_ViewZ(void) const {
		return m_fViewZ;}
	const _bool Get_isDead(void) const {
		return m_isDead;	}
	const _bool Get_isHitZone(void) const {
		return m_isHitZone;	}
	const _bool Get_isHitZoneFull(void) const {
		return m_isHitZoneFull;	}
public:
	void Set_isDead(const _bool& isDead) {
		m_isDead = isDead;	}
	void Set_HitZoneType(HITZONETYPE eType, const _uint& iSCENEID);
public:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
protected:
	LPDIRECT3DDEVICE9									m_pGraphic_Device = nullptr;
	unordered_map<const _tchar*, CComponent*>			m_mapComponent;
	typedef unordered_map<const _tchar*, CComponent*>	MAPCOMPONENT;
	_float												m_fViewZ = 0.f;
protected:
	CComponent_Manager*									m_pComponent_Manager = nullptr;
	CInput_Device*										m_pInput_Device = nullptr;
	CTexture*											m_pHitZomeTextureCom = nullptr;
protected:
	_bool												m_isClone = false;
	_bool												m_isDead = false;
	_bool												m_isCulling = false;
	_bool												m_isHitZone = false;
	_bool												m_isHitZoneFull = false;
	HITZONETYPE											m_eHitZoneType = TYPE_END;
protected:
	HRESULT Add_Component(const _tchar* pComponentTag, CComponent* pComponent);
	CComponent* Find_Component(const _tchar* pComponentTag);
	void Compute_ViewZ(const _vec3* pPosition);
public:
	virtual CGameObject* Clone(const _vec3* pPos = nullptr) _PURE;
	virtual CGameObject* Clone(const _matrix& matWorld) _PURE;
public:
	virtual void Free(void);
};

_END