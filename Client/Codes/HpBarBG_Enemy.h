#pragma once
#include "UI.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CRect_Texture;
	class CTexture;
	class CShader;
	class CCollider;
	class CObject_Manager;
}

class CUnit;
class CHpBarBG_Enemy final : public CUI
{
private:
	explicit CHpBarBG_Enemy(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CHpBarBG_Enemy(const CHpBarBG_Enemy& rhs);
	virtual ~CHpBarBG_Enemy(void) = default;
private:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(void);
public:
	void Set_UnitHpBar(CUnit* pUnit);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRect_Texture*			m_pBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
private:
	CUnit*					m_pLinkedUnit = nullptr;
	_float					m_fHpX = 0.f;
public:
	static CHpBarBG_Enemy* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

