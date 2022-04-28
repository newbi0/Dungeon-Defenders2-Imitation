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
class CHpBar_Player final : public CUI
{
private:
	explicit CHpBar_Player(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CHpBar_Player(const CHpBar_Player& rhs);
	virtual ~CHpBar_Player(void) = default;
private:
	virtual HRESULT Ready_GameObject_Prototype(const _bool& isLeft);
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
	_bool					m_isLeft = false;
private:
	CUnit*					m_pLinkedUnit = nullptr;
	_float					m_fHpY = 0.f;
public:
	static CHpBar_Player* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _bool& isLeft);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

