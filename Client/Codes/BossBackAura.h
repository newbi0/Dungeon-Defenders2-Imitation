#pragma once
#include "Skill.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CRect_Texture;
	class CTexture;
	class CShader;
}

class CBossBackAura final : public CSkill
{
private:
	explicit CBossBackAura(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBossBackAura(const CBossBackAura& rhs);
	virtual ~CBossBackAura(void) = default;
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
	CRect_Texture*			m_pBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
private:
	_float					m_fRot = 0.f;
public:
	static CBossBackAura* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

