#pragma once
#include "GameObject.h"

_BEGIN(Engine)

class _ENGINE_DLL CSkill abstract :
	public CGameObject
{
protected:
	explicit CSkill(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSkill(const CGameObject& rhs);
	virtual ~CSkill(void) = default;
public:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
public:
	virtual CGameObject* Clone(const _vec3* pPos = nullptr) _PURE;
	virtual CGameObject* Clone(const _matrix& matWorld) _PURE;
protected:
	virtual void Free(void);
};

_END