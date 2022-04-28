#pragma once
#include "Weapon.h"

class CSword final : public CWeapon
{
private:
	explicit CSword(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSword(const CSword& rhs);
	virtual ~CSword(void) = default;
private:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	virtual HRESULT Ready_Component(void);
	virtual	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
private:
	virtual void Update_Collider(void);
private:
	virtual void Collision_Att(const _tchar* pTargetTag);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
public:
	static CSword* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

