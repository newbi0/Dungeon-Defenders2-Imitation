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

class CFont_Damage final : public CUI
{
private:
	explicit CFont_Damage(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CFont_Damage(const CFont_Damage& rhs);
	virtual ~CFont_Damage(void) = default;
public:
	void Set_Font(const _uint& iDamage);
private:
	virtual HRESULT Ready_GameObject_Prototype(void);
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
	CRect_Texture*			m_pBufferCom_0 = nullptr;		//첫번째 자리
	CRect_Texture*			m_pBufferCom_1 = nullptr;		//두번째 자리
	CRect_Texture*			m_pBufferCom_2 = nullptr;		//세번째 자리
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
private:
	_vec3					m_vWholePos = _vec3(0.f, 0.f, 0.f);
	_uint					m_iDamege_0 = 0;		//첫번째 자리
	_uint					m_iDamege_1 = 0;		//두번째 자리
	_uint					m_iDamege_2 = 0;		//세번째 자리
public:
	static CFont_Damage* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

