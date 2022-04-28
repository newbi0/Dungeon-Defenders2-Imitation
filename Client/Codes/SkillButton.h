#pragma once
#include "UI.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CRect_Texture;
	class CTexture;
	class CShader;
}

class CSkillButton final : public CUI
{
private:
	explicit CSkillButton(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSkillButton(const CSkillButton& rhs);
	virtual ~CSkillButton(void) = default;
private:
	virtual HRESULT Ready_GameObject_Prototype(const _uint& iIdx);
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
	_uint					m_iIndex = 0;
public:
	static CSkillButton* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _uint& iIdx);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

