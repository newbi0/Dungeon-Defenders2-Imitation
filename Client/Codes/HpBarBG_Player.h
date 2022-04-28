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

class CHpBarBG_Player final : public CUI
{
private:
	explicit CHpBarBG_Player(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CHpBarBG_Player(const CHpBarBG_Player& rhs);
	virtual ~CHpBarBG_Player(void) = default;
private:
	virtual HRESULT Ready_GameObject_Prototype(const _bool& isLeft);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(void);
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
public:
	static CHpBarBG_Player* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _bool& isLeft);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

