#pragma once
#include "Skill.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CRect_Texture;
	class CTexture;
	class CShader;
	class CObject_Manager;
}

class CThunderbolt final : public CSkill
{
private:
	explicit CThunderbolt(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CThunderbolt(const CThunderbolt& rhs);
	virtual ~CThunderbolt(void) = default;
public:
	void Set_TargetPos(const _vec3& vTargetPos) {
		m_vTargetPos = vTargetPos;	}
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
	CObject_Manager*		m_pObject_Manager = nullptr;
private:
	CGameObject*			m_pTarget = nullptr;
	_vec3					m_vTargetPos;
private:
	_uint					m_iTextureNum = 0;
public:
	static CThunderbolt* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

